/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project5-1
edit : wwChoi
---------------------------------------------------------------------------------------------
[ CAN수신 ]
Experiment1 -> CAN수신된 값을 Y축 값을 받아 DC Motor를 Control한다.
Experiment2 -> CAN수신된 값을 X축 값을 받아 Servo Motor를 Control한다.

*** 각 실험에서는 LCD를 이용하여 수신된 값을 Display한다.
--------------------------------------------------------------------------------------------*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>         // sprintf문 선언되어 있음 

#include "myDelay.h"
#include "lcdControl.h"
#include "myCANLib.h"

#include "lcdControl.c"
#include "myCANLib.c"

#define FREQ_CLKIO      16000000   // clock IO 16 MHz
#define PRESCALE      64
#define RC_SERVO_PERIOD   25         // RC servo 모터 주기 25 msec

unsigned int voltage_x, voltage_y;

struct MOb msg1;    

char strBuff[20]={0};

unsigned char engineFlag=0;






void initPort(void)
{
   DDRC  = 0xff;
   PORTC = 0xff;
   DDRG  = 0xff;
   DDRF  = 0xfc;
   DDRE  = 0xff;
}

// motor control ======================================================
void initMotor(void)
{
   TCCR3A= 0b10001010;
   TCCR3B= 0x13;
   ICR3 = FREQ_CLKIO/2/PRESCALE/1000*RC_SERVO_PERIOD;   
               // 최고값(Top)3125, 40Hz(25msec) 

   OCR3C= 70;      // 최저값(output compare) PE5 pin output

}

// DC모터 속도 제어와 모터 회전 방향 제어 
void CtrlDcMotor(unsigned int speed, unsigned int dir)
{
   unsigned int level=5;

   PORTE&=0xFC;

   // DC모터 회전 방향 결정
   if(dir==0)      // 시계 방향 회전
   {
      // speed에 따른 속도 제어
      OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*RC_SERVO_PERIOD)/level;
      PORTE|=0x01;   
   }
   else if(dir==1)   // 반시계 방향 회전 
   {
      // speed에 따른 속도 제어
      OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*RC_SERVO_PERIOD)/level;
      PORTE|=0x02;
   }
   else         // 모터 정지 
   {
      OCR3A=100;
      OCR3B=100;
   }
}

// 서보 모터 회전 제어(degree : 각도) 
void CtrlRcServoMotor(unsigned int degree)
{
   OCR3C= FREQ_CLKIO/2/PRESCALE/1000*(1.6/180*degree+(0.15/180*degree+0.6));
}

// 조이스틱 Y축의 AD 변화값에 따른 모터 속도 설정 
unsigned int DcMotorSpeed(unsigned int volt_y)
{
   unsigned int speed=0;

   if (volt_y<10) speed=5;
   else if (volt_y<14) speed=4;
   else if (volt_y<18) speed=3;
   else if (volt_y<22) speed=2;
   else if (volt_y<26) speed=1;
   else if (volt_y<28) speed=1;
   else if (volt_y<32) speed=2;
   else if (volt_y<36) speed=3;
   else if (volt_y<40) speed=4;
   else if (volt_y<55) speed=5;

   return speed;
}   

// 조이스틱 Y축의 AD 변화값에 따른 DC모터 회전 방향 결정    
unsigned int DcMotorDir(unsigned int volt_y)
{
   unsigned int dir=0;

   if(volt_y<25) dir=0;      // 시계 방향 
   else if(volt_y>28) dir=1;   // 반시계 방향 
   else dir=2;               // 정지 

   return dir;
}

// 조이스틱 X축의 AD 변화값에 따른 서보 모터 회전각 결정 
unsigned int RcServoMotorAngle(unsigned int volt_x)
{
   unsigned int angle;
   if(volt_x>25 && volt_x<29) angle=90;
   else angle=(volt_x)*180/50;
   
   return angle;
}

void us_delay(unsigned int us_time)
{
   unsigned int i;

   for(i=0; i<us_time; i++) // 4 cycle +
   {
      asm("PUSH R0");    // 2 cycle +
      asm("POP R0");       // 2 cycle +
      asm("PUSH R0");    // 2 cycle +
      asm("POP R0");       // 2 cycle + =12 cycle for 11.0592MHZ
      asm("PUSH R0");    // 2 cycle +
      asm("POP R0");       // 2 cycle = 16 cycle = 1us for 16MHz
   }
}

void ms_delay(unsigned int ms_time)
{
    unsigned int i;
    
    for(i=0; i<ms_time;i++)
        us_delay(1000);
}


int main(void)
{
   unsigned int angle;
   unsigned int dir;
   unsigned int speed;

   initPort();      // 입출력 포트 초기화

    LCD_init();     // LCD 초기화
    
   initMotor();   // 스텝모터 제어를 위한 타이머/카운터 초기화 

   
   can_init(b250k);       // 초기화
   can_rx_set(1, 0x01,EXT,8 ,0x00,0b0000);    // CAN 수신기 초기화 
//(char obj, unsigned long id, char ide, unsigned char dlc, 
//            unsigned long idmask, unsigned char rtrIdemask)   

   sei();         // INT 인에이블 

   while(1)
   {
      char temp1[5], temp2[5], temp3[5];
      can_rx(1, &msg1);
      LCD_Write(0,0,"Y : ");
	  sprintf(temp1, "%d", msg1.data[0]);
      LCD_Write(4,0,temp1);
      LCD_Write(0,1,"X : ");
	  sprintf(temp2, "%d", msg1.data[1]);
      LCD_Write(4,1,temp2);
      /********************************************
      msg1.data[] 의 배열을 참고
      can_rx()함수 이용하여 voltage_x,voltage_y 수신하여 모터 컨트롤
      LCD에 수신 X축,Y축값 표시
      CtrlRcServoMotor() -> 서보모터 Control / DcMotorDir()이용하여 회전방향 결정
      CtrlDcMotor() -> DC모터 Control / RcServoMotorAngle()이용하여 각도 결정
      ********************************************/
	  angle = RcServoMotorAngle(msg1.data[1]);
  	  CtrlRcServoMotor(angle);
      CtrlDcMotor(DcMotorSpeed(msg1.data[0]), DcMotorDir(msg1.data[0]));

   }
}
