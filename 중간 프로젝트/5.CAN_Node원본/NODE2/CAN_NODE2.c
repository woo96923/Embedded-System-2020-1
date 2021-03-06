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

//#include "lcdControl.c"
//#include "myCANLib.c"

#define FREQ_CLKIO      16000000   // clock IO 16 MHz
#define PRESCALE      64
#define RC_SERVO_PERIOD   25         // RC servo 모터 주기 25 msec

unsigned int voltage_x, voltage_y;


struct MOb msg1;    

char strBuff[20]={0};

unsigned char engineFlag=0;
char table1[15] = "CURRENT MODE : ";

volatile int count;

ISR(TIMER1_OVF_vect) //TCNT1 값이 넘치면 (오버플로우가 발생하면) 이 서브 루틴으로 점프합니다.
{
    if(count>50)  
    count = 0;
    else 
    count++;
    
    TCNT1H = 0xFB; //1초의 오버플로우를 만들기 위한 타이머/카운터1의 초기값을 정해줍니다.
    TCNT1L = 0x1D;          
}



void initPort(void)
{
   DDRC  = 0xff;
   PORTC = 0x00;
   DDRG  = 0x0f;
   DDRF  = 0x00;
   DDRE  = 0xff;
   DDRB =  0xff;
}

// motor control ======================================================
void initMotor(void)
{
   TCCR3A=0b10001010;
   TCCR3B=0b11010011;
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

   

// 조이스틱 Y축의 AD 변화값에 따른 DC모터 회전 방향 결정    
unsigned int DcMotorDir(unsigned int mode)
{
   unsigned int dir=0;

   if( mode == 3) dir=0;      // driving, 시계 방향 
   else if(mode == 1) dir=1;   // reverse 반시계 방향 
   else dir=2;               // p, n 정지 

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



int main(void)
{
   unsigned int angle;
   unsigned int dir;
   unsigned int speed;
   int twink = 0;
   //////
   TCCR1B = 0b00000100;
   TIMSK1 = (1<<TOIE1);
   TCNT1H = 0xFB;
   TCNT1L = 0x1D;

   initPort();      // 입출력 포트 초기화

    LCD_init();     // LCD 초기화

   initMotor();   // 스텝모터 제어를 위한 타이머/카운터 초기화 

   
   can_init(  5 );       // 초기화
   can_init_8Mhz(5);
   can_rx_set( 1, 0x01, EXT, 8, 0x00, 0x05  );    // CAN 수신기 초기화 
   
   
   sei();         // INT 인에이블 
   //LCD_cmd(0x01);
   LCD_Write(0,0, "before");
   ms_delay(500);
   int current_mode = 0;
   int current_speed = 0;
   int past_mode = 0;
   int past_speed = 0;
   PORTB = 0xff;
   while(1)
   {
      /********************************************
      /* NODE 2
      /* get Car Mode from NODE1
      /* control DC, Servo motor
      /* 
      ********************************************/


      LCD_Write(0,0, table1);
       can_rx(1, &msg1);
      LCD_Write(0, 1, &(msg1.data[4]));
      LCD_Write(3, 1, &(msg1.data[6]));
      //speed & dir control
      count = 0;
      CtrlDcMotor( msg1.data[2], DcMotorDir(msg1.data[0]));

      if(msg1.data[0] == 3) {

         while( count <= 50);
         CtrlRcServoMotor(90);

      }
      else if(msg1.data[0] == 0 ) CtrlRcServoMotor(0);

      
      if( ( msg1.data[6] == '1' ) || ( msg1.data[6] == '3')) {
         twink = 1;
      }
      else twink = 0;
      if(twink == 1) {
         PORTB = 0x00;

      }
      else PORTB = 0xff;

   }
}
