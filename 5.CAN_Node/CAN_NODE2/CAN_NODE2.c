/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project5-1
edit : wwChoi
---------------------------------------------------------------------------------------------
[ CAN���� ]
Experiment1 -> CAN���ŵ� ���� Y�� ���� �޾� DC Motor�� Control�Ѵ�.
Experiment2 -> CAN���ŵ� ���� X�� ���� �޾� Servo Motor�� Control�Ѵ�.

*** �� ���迡���� LCD�� �̿��Ͽ� ���ŵ� ���� Display�Ѵ�.
--------------------------------------------------------------------------------------------*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>         // sprintf�� ����Ǿ� ���� 

#include "myDelay.h"
#include "lcdControl.h"
#include "myCANLib.h"

#include "lcdControl.c"
#include "myCANLib.c"

#define FREQ_CLKIO      16000000   // clock IO 16 MHz
#define PRESCALE      64
#define RC_SERVO_PERIOD   25         // RC servo ���� �ֱ� 25 msec

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
               // �ְ�(Top)3125, 40Hz(25msec) 

   OCR3C= 70;      // ������(output compare) PE5 pin output

}

// DC���� �ӵ� ����� ���� ȸ�� ���� ���� 
void CtrlDcMotor(unsigned int speed, unsigned int dir)
{
   unsigned int level=5;

   PORTE&=0xFC;

   // DC���� ȸ�� ���� ����
   if(dir==0)      // �ð� ���� ȸ��
   {
      // speed�� ���� �ӵ� ����
      OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*RC_SERVO_PERIOD)/level;
      PORTE|=0x01;   
   }
   else if(dir==1)   // �ݽð� ���� ȸ�� 
   {
      // speed�� ���� �ӵ� ����
      OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*RC_SERVO_PERIOD)/level;
      PORTE|=0x02;
   }
   else         // ���� ���� 
   {
      OCR3A=100;
      OCR3B=100;
   }
}

// ���� ���� ȸ�� ����(degree : ����) 
void CtrlRcServoMotor(unsigned int degree)
{
   OCR3C= FREQ_CLKIO/2/PRESCALE/1000*(1.6/180*degree+(0.15/180*degree+0.6));
}

// ���̽�ƽ Y���� AD ��ȭ���� ���� ���� �ӵ� ���� 
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

// ���̽�ƽ Y���� AD ��ȭ���� ���� DC���� ȸ�� ���� ����    
unsigned int DcMotorDir(unsigned int volt_y)
{
   unsigned int dir=0;

   if(volt_y<25) dir=0;      // �ð� ���� 
   else if(volt_y>28) dir=1;   // �ݽð� ���� 
   else dir=2;               // ���� 

   return dir;
}

// ���̽�ƽ X���� AD ��ȭ���� ���� ���� ���� ȸ���� ���� 
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

   initPort();      // ����� ��Ʈ �ʱ�ȭ

    LCD_init();     // LCD �ʱ�ȭ
    
   initMotor();   // ���ܸ��� ��� ���� Ÿ�̸�/ī���� �ʱ�ȭ 

   
   can_init(b250k);       // �ʱ�ȭ
   can_rx_set(1, 0x01,EXT,8 ,0x00,0b0000);    // CAN ���ű� �ʱ�ȭ 
//(char obj, unsigned long id, char ide, unsigned char dlc, 
//            unsigned long idmask, unsigned char rtrIdemask)   

   sei();         // INT �ο��̺� 

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
      msg1.data[] �� �迭�� ����
      can_rx()�Լ� �̿��Ͽ� voltage_x,voltage_y �����Ͽ� ���� ��Ʈ��
      LCD�� ���� X��,Y�ప ǥ��
      CtrlRcServoMotor() -> �������� Control / DcMotorDir()�̿��Ͽ� ȸ������ ����
      CtrlDcMotor() -> DC���� Control / RcServoMotorAngle()�̿��Ͽ� ���� ����
      ********************************************/
	  angle = RcServoMotorAngle(msg1.data[1]);
  	  CtrlRcServoMotor(angle);
      CtrlDcMotor(DcMotorSpeed(msg1.data[0]), DcMotorDir(msg1.data[0]));

   }
}
