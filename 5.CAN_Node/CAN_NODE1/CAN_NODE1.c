/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project5-1
edit : wwChoi
---------------------------------------------------------------------------------------------
[ CAN�۽� ]

Experiment1 -> : ���̽�ƽ�� �̿��Ͽ� X��, Y�ప�� CAN�� �̿��Ͽ� �����Ѵ�. ( LCD�� �ڽ��� ������ �����͸� ǥ���Ѵ�. )

--------------------------------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>         // sprintf�� ����Ǿ� ���� 

#include "myDelay.h"
#include "lcdControl.h"
#include "myCANLib.h"

#include "myDelay.c"
#include "lcdControl.c"
#include "myCANLib.c"

unsigned int ADdata; 
unsigned int voltage_x, voltage_y;
unsigned int volume_x, volume_y;
unsigned char ADCFlag;

struct MOb msg1={0x01, 0, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};    
struct MOb msg2;   


char strBuff[20]={0};





//=========================================================
// AD ��ȯ 
// PF0: ADC0 ���̽�ƽ y�� 
// PF1: ADC1 ���̽�ƽ x��
//=========================================================

void initAdc(void){
      ADMUX = 0x40;   // ��������; �ܺ� ĳ�۽��� ���� AVcc(AREF ��)
               // AD��ȯ ������ ����; ������ ���� 
               // AD��ȯ ä�� ����; PortF 0 ��    

   DDRF  = 0xf0;   // PortF[3..0] �Է����� ����, PortF[7..4] ������� ���� 
   DIDR0 = 0x0f;   // ������ �Է� �Ұ� PortF[3..0]

      ADCSRA= 0xc8;    // ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺� 
               // ADC Ŭ�� ����; XTAL��1/2(8MHz)
}

SIGNAL(ADC_vect)
{
	ADdata = ADC;
	ADCSRA = 0xc8;
}

void ADcLCD(void)
{
   if(ADCFlag == 0)
   { 
      volume_y= ADdata; // �������� PortF0
      voltage_y= volume_y*50/1023;
      ADCFlag=1; 
      ADMUX = 0x40;   // PortF1 �Ƴ��α� �Է� ���� 
   } 
   else if(ADCFlag == 1)
   { 
      volume_x= ADdata; // �µ����� PortF1
      voltage_x= volume_x*50/1023;
      ADCFlag=0; 
      ADMUX = 0x41;   // PortF2 �Ƴ��α� �Է� ���� 
   }
   else ADCFlag=0;
}

void initPort(void)
{
   DDRC  = 0xff;   // LCD ������ �� ��� 
   PORTC = 0x00;
   DDRG  = 0x0f;   // LCD ���� ��� (RS, RW, E)
   DDRF  = 0x00;   // �Ƴ��α� �Է� 

   DDRE  = 0x00;
}



int main(void)
{
   initPort();      // ����� ��Ʈ �ʱ�ȭ
    LCD_init();     // LCD �ʱ�ȭ
   can_init(b250k);   // CAN ������Ʈ�� ���ϴ� ������ �����Ѵ�.
   initAdc();      // AD ��ȯ �ʱ�ȭ
   sei();         // INT �ο��̺� 

   while(1)
   {

      
      /********************************************
      ����1 �ҽ��ڵ�
      ADcLCD()�Լ� �̿�
      msg1.data[]�迭 �̿� 
       can_tx() �Լ� �̿�
      ********************************************/
	  char temp1[5], temp2[5];
	  ADcLCD();
	  sprintf(temp1, "%d", voltage_y);
	  ADcLCD();
	  msg1.data[0] = voltage_y;
	  sprintf(temp2, "%d", voltage_x);
	  msg1.data[1] = voltage_x;
	  LCD_Write(0,0,"Y : ");
      LCD_Write(4,0,temp1);
      LCD_Write(0,1,"X : ");
      LCD_Write(4,1,temp2);
	  
      
      can_tx(1,&msg1,0);
   }
}
