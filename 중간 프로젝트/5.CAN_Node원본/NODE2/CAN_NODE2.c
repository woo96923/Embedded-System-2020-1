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

//#include "lcdControl.c"
//#include "myCANLib.c"

#define FREQ_CLKIO      16000000   // clock IO 16 MHz
#define PRESCALE      64
#define RC_SERVO_PERIOD   25         // RC servo ���� �ֱ� 25 msec

unsigned int voltage_x, voltage_y;


struct MOb msg1;    

char strBuff[20]={0};

unsigned char engineFlag=0;
char table1[15] = "CURRENT MODE : ";

volatile int count;

ISR(TIMER1_OVF_vect) //TCNT1 ���� ��ġ�� (�����÷ο찡 �߻��ϸ�) �� ���� ��ƾ���� �����մϴ�.
{
    if(count>50)  
    count = 0;
    else 
    count++;
    
    TCNT1H = 0xFB; //1���� �����÷ο츦 ����� ���� Ÿ�̸�/ī����1�� �ʱⰪ�� �����ݴϴ�.
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

   

// ���̽�ƽ Y���� AD ��ȭ���� ���� DC���� ȸ�� ���� ����    
unsigned int DcMotorDir(unsigned int mode)
{
   unsigned int dir=0;

   if( mode == 3) dir=0;      // driving, �ð� ���� 
   else if(mode == 1) dir=1;   // reverse �ݽð� ���� 
   else dir=2;               // p, n ���� 

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

   initPort();      // ����� ��Ʈ �ʱ�ȭ

    LCD_init();     // LCD �ʱ�ȭ

   initMotor();   // ���ܸ��� ��� ���� Ÿ�̸�/ī���� �ʱ�ȭ 

   
   can_init(  5 );       // �ʱ�ȭ
   can_init_8Mhz(5);
   can_rx_set( 1, 0x01, EXT, 8, 0x00, 0x05  );    // CAN ���ű� �ʱ�ȭ 
   
   
   sei();         // INT �ο��̺� 
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
