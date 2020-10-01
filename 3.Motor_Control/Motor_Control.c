/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project3
edit : wwChoi
---------------------------------------------------------------------------------------------

 Experiment1 ->  SW2 �����ÿ� Motor �۵�, SW3������ Motor ����
 Experiment2 ->  SW2 �����ÿ� Motor Speed UP, SW3�����ÿ� Speed Down[ Motor Speed down �ִ�ӵ� 5���� ]
--------------------------------------------------------------------------------------------*/
#define Experiment 2

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>         

#include "lcdControl.h"



#define FREQ_CLKIO      16000000   // clock IO 16 MHz
#define PRESCALE      64
#define DC_PERIOD      25         // DC ���� �ֱ� 25 msec

#define BUTTON0_MASK 0x01
#define BUTTON1_MASK 0x02

#define SPEED_NO_CAHNGE   0
#define SPEED_UP       1
#define SPEED_DOWN       2
#define SPEED_MAX      5
#define SPEED_MIN      0


unsigned char table1[16]="AJOU-UNIVERSITY";
#if Experiment == 1
unsigned char table2[16]="ON_OFF_CONTROL";
#elif Experiment ==2
unsigned char table2[16]="SPEED_CONTROL";
#endif

unsigned char strBuff[20]={0};


void initPort(void)
{
   DDRC  = 0xff;   // LCD ������ �� ��� 
   DDRG  = 0x0f;   // LCD ���� ��� (RS, RW, E)
   DDRD  = 0x00;   // SW �Է¼���
   DDRE  = 0xff;   // Motor�� �̿��ϱ� ���� ��Ʈ. �ʿ��� �� ��� ����
}

// DC���� �ʱ�ȭ
void initMotor(void)
{
   TCCR3A=   0b10000010;   // COM3A[1:0]=10,  ����ġ�� Ŭ����
            //               TOP �� 
            // WGM3[3:0] :     Phase correct PWM mode 
            //                 TOP���� ICR �������Ͱ� ����
   TCCR3B=   0b00010011;   // 64 ���� 
                
                
   ICR3 = FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD;   
               // �ְ�(Top)3125, 40Hz(25msec) 
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
      OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD)/level;
      PORTE|=0x01;   
   }
   else if(dir==1)   // �ݽð� ���� ȸ�� 
   {
      // speed�� ���� �ӵ� ����
      OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD)/level;
      PORTE|=0x02;
   }
   else         // ���� ���� 
   {
      OCR3A=100;
      OCR3B=100;
   }
}


#if Experiment == 1
unsigned char* ButtonInput(void)
{
   unsigned char in;
   unsigned char up;
   unsigned char down;
   unsigned char* Button_Info;
      
   // ����ũ�� �̿��� ��ƮD �Է¿��� 0���� �Է� ���� 
   in = PIND & BUTTON0_MASK;

   if(in>0) up=0;   // S2��ư �� ������ 
   else up=1;      // S2��ư ������

   // ����ũ�� �̿��� ��ƮD �Է¿��� 1���� �Է� ���� 
   in = PIND & BUTTON1_MASK;

   if(in>0) down=0;   // S3��ư �� ������   
   else down=1;      // S3��ư ������

   if(up==1 && down==0) Button_Info= " ON";   // S2��ư ���� ���
   else if(up==0 && down==1) Button_Info = "OFF";   // S3��ư ���� ���
   
   return Button_Info;
}

#elif Experiment == 2
// ����D�� 0��(S3)�� 1��(S2) �ɿ��� �Է� ���� 
unsigned char ButtonInput(void)
{
   unsigned char in;
   unsigned char up;
   unsigned char down;
      
   // ����ũ�� �̿��� ��ƮD �Է¿��� 0���� �Է� ���� 
   in = PIND & BUTTON0_MASK;

   if(in>0) up=0;   // S2��ư �� ������ 
   else up=1;      // S2��ư ������

   // ����ũ�� �̿��� ��ƮD �Է¿��� 1���� �Է� ���� 
   in = PIND & BUTTON1_MASK;

   if(in>0) down=0;   // S3��ư �� ������   
   else down=1;      // S3��ư ������

   if(up==1 && down==0) return SPEED_UP;   // S2��ư ���� ���
   if(up==0 && down==1) return SPEED_DOWN;   // S3��ư ���� ���

   return SPEED_NO_CAHNGE;
}
#endif

// ���� �ӵ��� ���� �Լ� (��ư �Է¿� ����)
unsigned char DcMotorSpeedControl(unsigned char bt_in, unsigned char speed)
{
   // S2 ��ư�� �������� 
   if(bt_in==SPEED_UP) 
   {
      if(speed==SPEED_MAX) return speed;
      return (++speed);   // �ӵ� 1 ���� 
   }
   // S3 ��ư�� ��������
   if(bt_in==SPEED_DOWN)
   {
      if(speed==SPEED_MIN) return speed;
      return (--speed);   // �ӵ� 1 ���� 
   }
   return speed;
}

void delay(unsigned int k)
{
    unsigned int i;

   for(i=0;i<k;i++); 
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
#if Experiment == 1
   unsigned char* out;
   unsigned int speed=0;

   initPort();   
   LCD_init();
   initMotor();
   sei();                // ���� ÷�ν� �� �Լ��� ���� �����Ͻÿ�.

   LCD_cmd(0x01);         // LCD ȭ�� ����� 
   ms_delay(50);         // 50ms ���� 

   LCD_Write(0,0, table1);   // �ʱ� ���� ���
   LCD_Write(0,1, table2);

   ms_delay(1500);
   //LCD_cmd(0x01);         // LCD ȭ�� ����� 
   ms_delay(100);

	speed = 5;
   while(1)
   {   
      /*****************************************
       ����1 �ڵ��ۼ�
      : CPU������ ��ư S2(+)�� S3(-)�� �Է� ����    
      : ButtonInput()�Լ��� �̿��ϼ���.
      : CtrlDcMotor()�Լ��� �̿��ϼ���.
      
      ******************************************/
  /*    
	  out = ButtonInput();
      if (out == " ON")
         speed = 0;
      else if (out == "OFF")
         speed = 5;
   
   	  CtrlDcMotor(speed,0);
*/
		out = ButtonInput();
      speed = 5;
      if (out == " ON")
         CtrlDcMotor(speed, 0);
      else if (out == "OFF")
         CtrlDcMotor(speed, 2);

      ms_delay(100); // ��Ʈ : �����ӵ��� Display�� �ϰԵǸ� ������ �߻��Ѵ�.
   }



#elif Experiment == 2

   unsigned int speed=0;
   unsigned int out=0;
   unsigned int butten_check=0;

   initPort();      // ����� ��Ʈ �ʱ�ȭ

    LCD_init();     // LCD �ʱ�ȭ

   initMotor();   // ���ܸ��� ��� ���� Ÿ�̸�/ī���� �ʱ�ȭ 

   sei();         // ��������� ���ؼ� ������ ÷���ϱ�ٶ�.

   LCD_cmd(0x01);   // LCD ȭ�� ����� 
   ms_delay(50);   // 50ms ���� 

   LCD_Write(0,0, table1);   // �ʱ� ���� ���
   LCD_Write(0,1, table2);

   ms_delay(1500);
   //LCD_cmd(0x01);         // LCD ȭ�� ����� 
   ms_delay(100);
 
   unsigned char *button;
   unsigned char *state;
   unsigned int button_check=0;
   while(1)
   {   
      /*****************************************
       ����2 �ڵ��ۼ�
      : CPU������ ��ư S2(+)�� S3(-)�� �Է� ����    
      : ButtonInput()�Լ��� �̿��ϼ���.
      : CtrlDcMotor()�Լ��� �̿��ϼ���.
      : DcMotorSpeedControl()�Լ��� �̿��ϼ���.
      ******************************************/
     
    /*
    if ((out=="SPEED_UP"||out=="SPEED_DOWN")&&butten_check==0) 
    {
    butten_check=1;
    
     speed = DcMotorSpeedControl(out, speed);
    }
    if (out=="SPEED_NO_CHANGE"&&butten_check==1) butten_check=0;
      */
   
   button = ButtonInput();

   if (button != state) button_check = 0;

   if (button_check == 0)
   {
      speed = DcMotorSpeedControl(button, speed);
      button_check = 1;
	  state = button;
   }

   CtrlDcMotor(speed,0);
     
    char temp[2];
     sprintf(temp,"%d",speed);
     LCD_Write(13,1, temp);



      ms_delay(100);
   }

#endif
}
