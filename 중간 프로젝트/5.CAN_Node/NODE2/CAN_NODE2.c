/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project3
edit : wwChoi
---------------------------------------------------------------------------------------------

 Experiment1 ->  SW2 �����ÿ� Motor �۵�, SW3������ Motor ����
 Experiment2 ->  SW2 �����ÿ� Motor Speed UP, SW3�����ÿ� Speed Down[ Motor Speed down �ִ�ӵ� 5���� ]
--------------------------------------------------------------------------------------------*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>         

#include "lcdControl.h"
#include "myCANLib.h"
#include "myCANLib.c"


#define FREQ_CLKIO      16000000   // clock IO 16 MHz
#define PRESCALE      64
#define DC_PERIOD      25         // DC ���� �ֱ� 25 msec

#define BUTTON0_MASK 0x01
#define BUTTON1_MASK 0x02

#define SPEED_NO_CHANGE   0
#define SPEED_UP       1
#define SPEED_DOWN       2
#define SPEED_MAX      5
#define SPEED_MIN      0

#define PE4_Clear   (PORTE &= 0xEF)
#define PE4_Set      (PORTE |= 0x10)
#define PE4_IN      (DDRE &= 0xEF)
#define PE4_OUT      (DDRE |= 0x10)

#define TEMPERATURE 25

unsigned short tick=0, pulse_check=0, pulse_end=0;

unsigned char table1[16]="AJOU-UNIVERSITY";
unsigned char table2[16]="SPEED_CONTROL";

char *mode[3][12] = { "WAIT", "ACCR", "SLOW"};

unsigned char strBuff[20]={0};

unsigned char distenceCheck=1;

struct MOb msg1;/////////////
struct MOb msg3 = {3, 0x03, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};////////////


void initPort(void)
{
   DDRC  = 0xff;   // LCD ������ �� ��� 
   DDRG  = 0x0f;   // LCD ���� ��� (RS, RW, E)
   DDRD  = 0x00;   // SW �Է¼���
   DDRE  = 0xf0;   // Motor�� �̿��ϱ� ���� ��Ʈ. �ʿ��� �� ��� ����
   PORTE = 0xff;
}

// DC���� �ʱ�ȭ
void initMotor(void)
{
   TCCR3A=   0b10001010;   // COM3A[1:0]=10,  ����ġ�� Ŭ����
            //               TOP �� 
            // WGM3[3:0] :     Phase correct PWM mode 
            //                 TOP���� ICR �������Ͱ� ����
   TCCR3B=   0b00010011;   // 64 ���� 
                
                
   ICR3 = FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD;   
               // �ְ�(Top)3125, 40Hz(25msec) 

   OCR3C = 70;
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

   if((up==1) && (down==0)) return SPEED_UP;   // S2��ư ���� ���
   if((up==0) && (down==1)) return SPEED_DOWN;   // S3��ư ���� ���

   return SPEED_NO_CHANGE;
}


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


// ultra sound 
void init_EXT_INT(void)
{
   EIMSK |= (1<<INT4);      // INT4 Interrupt Set
   EICRB |= (1<<ISC41)|(1<<ISC40);      //INT4 Rising Edge /INT Set
   EIFR   = 1<<INT4;      // Interrupt Flag
}

void init_TIMER0_COMPA(void)
{
   TCCR0A |= (1<<WGM01);      // CTC Mode
   TCCR0A |= (1<<CS01);   // clk/8 Prescaler
   TCNT0 = 0;
   OCR0A = 19;      // 1cycle -> 20us���� ���� 1cycle = 1/(16Mhz/(2*8*( OCR0A+1)))
   TIMSK0 = (1<<OCIE0A);      //T/C0 Output Compare Match INT
   TIFR0 = 0x00;
}

SIGNAL(TIMER0_COMP_vect)   
{
   tick++;
}

SIGNAL(INT4_vect)
{
   unsigned short pulse_tick;

   pulse_tick = tick;

   if(EICRB & 0x03)
   {
      EICRB &= 0x00;   //  low state
      tick = 0;   
   }
   else
   {
      EICRB |= (1<<ISC41)|(1<<ISC40);// INT4 Rising Edge / INT Set
      pulse_end = pulse_tick;   
   }
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
   unsigned int speed=3;
   unsigned int mode = 0; //0�� wait, 1�� speed up, 2�� speed down
   unsigned char thousand, hundred, ten, one;
   float distance, distance2;
   unsigned char position = 0;
   unsigned char distenceCheck=0;
   unsigned int dir = 2;
   unsigned int accTime = 0;
   unsigned int tempTime = 0;
   char temp[6];

   unsigned int firstMove = 0;


   unsigned char *button;
   can_init(5);       // �ʱ�ȭ
   can_init_8Mhz(5);
   can_rx_set(1, 0x01, EXT, 8, 0x00, 0x05  );       // CAN ���ű� �ʱ�ȭ 



   initPort();      // ����� ��Ʈ �ʱ�ȭ
   init_EXT_INT();
   init_TIMER0_COMPA();

    LCD_init();     // LCD �ʱ�ȭ

   initMotor();   // ���ܸ��� ��� ���� Ÿ�̸�/ī���� �ʱ�ȭ 

   sei();         // ��������� ���ؼ� ������ ÷���ϱ�ٶ�.

   LCD_cmd(0x01);   // LCD ȭ�� ����� 
 
  
      
   while(1)
   {
   /****************************************
   : ������ ������ ����ϱ� ���� �ҽ��ڵ� ����
   ****************************************/
      cli();            // clear interrupt
      PE4_Clear;
      PE4_OUT;         // PE4 pin is output
      us_delay(500);

      PE4_Set;         // output set during 5us
      us_delay(5);

      PE4_Clear;         // PE4 clear
      us_delay(100);

      PE4_IN;            // PE4 pin is input
      us_delay(100);

      sei();            // set interrupt


   /* distance = velocity * time */
   distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;
   

   /* distance digit display */
      thousand = distance/1000;
      distance = distance - (thousand * 1000);
      distance2 = distance;
   
      hundred = distance /100;
      distance = distance - (hundred * 100);

      ten = distance/10; 
      distance = distance - (ten * 10);

      one = distance;

   char part[5];
   part[0] = thousand + '0';
   part[1] = hundred + '0';
   part[2] = ten + '0';
   part[3] = one + '0';
   part[4] = '\0';
   LCD_Write(10,0, part);

   can_rx(1, &msg1);/////////////////////////////////

   mode = (int)msg1.data[0];///
   LCD_Write(6,0, &msg1.data[0]);

   button = ButtonInput(); 
   if(button == SPEED_UP)
      {mode = 1; LCD_Write(5,0, "UP");
	   firstMove = 1;}
   else if(button == SPEED_DOWN)
      {mode = 2; LCD_Write(5,0, "DN");}

   if (mode == 0) LCD_Write(0,0, "WAIT");//��� ���
   if (mode == 1) LCD_Write(0,0, "SPUP");//��� ���
   if (mode == 2) LCD_Write(0,0, "SPDN");//��� ���

   if (position == 0) LCD_Write(10,1, "R");
   if (position == 1) LCD_Write(10,1, "L");

   


    sprintf(temp,"%d",accTime);
    LCD_Write(0,1, temp);
    sprintf(temp,"%d",tempTime);
    LCD_Write(4,1, temp);
    
  
	  speed = 3;
      if (firstMove == 1)
	  {
	    firstMove = 0;
		tempTime = 0;
	  	CtrlDcMotor(speed, dir);
		delay(300);
	  }
	  else{
	  
		  if(distance2 < 50)
	      {
	         distenceCheck = position;
	         tempTime += 1;

	         if(tempTime > 200) {mode = 0; dir =2; tempTime = 0;}
	      }
	      else if((distance2 > 100)&& (distance2 < 200) && (position == distenceCheck))
	      {
	         accTime = tempTime;
	         tempTime = 0;
	         if (position == 0)
	         {
	            position = 1;
	            if(mode == 1)//speedup
	            dir = 1;
	            else if(mode == 2)//speed down
	            dir = 0;
	         }
	         else if (position == 1)
	         {
	            position = 0;
	            if(mode == 1)//speedup
	            dir = 0;
	            else if(mode == 2)//speed down
	            dir = 1;
	         }
	      }
   
	   }
      
      


      CtrlDcMotor(speed, dir);

          //From MCU2 to MCU1
	 msg3.data[0] = mode;
     msg3.data[1] = '\0'; 
     msg3.data[2] = tempTime; 
     msg3.data[3] = '\0'; 
     msg3.data[4] = '\0'; 
     msg3.data[5] = '\0'; 
     msg3.data[6] = '\0';
     msg3.data[7] = '\0'; 

	if(mode != 0)
	  can_tx(3, &msg3, 0);


   }


}
