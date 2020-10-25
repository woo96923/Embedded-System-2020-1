/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project5-1
edit : wwChoi
---------------------------------------------------------------------------------------------
[ CAN송신 ]

Experiment1 -> : 조이스틱을 이용하여 X축, Y축값을 CAN을 이용하여 전송한다. ( LCD에 자신이 보내는 데이터를 표시한다. )

--------------------------------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>         // sprintf문 선언되어 있음 

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
// AD 변환 
// PF0: ADC0 조이스틱 y축 
// PF1: ADC1 조이스틱 x축
//=========================================================

void initAdc(void){
      ADMUX = 0x40;   // 기준전압; 외부 캐퍼시터 가진 AVcc(AREF 핀)
               // AD변환 데이터 정렬; 오른쪽 정렬 
               // AD변환 채널 선택; PortF 0 핀    

   DDRF  = 0xf0;   // PortF[3..0] 입력으로 설정, PortF[7..4] 출력으로 설정 
   DIDR0 = 0x0f;   // 디지털 입력 불가 PortF[3..0]

      ADCSRA= 0xc8;    // ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블 
               // ADC 클럭 설정; XTAL의1/2(8MHz)
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
      volume_y= ADdata; // 가변저항 PortF0
      voltage_y= volume_y*50/1023;
      ADCFlag=1; 
      ADMUX = 0x40;   // PortF1 아날로그 입력 선택 
   } 
   else if(ADCFlag == 1)
   { 
      volume_x= ADdata; // 온도센서 PortF1
      voltage_x= volume_x*50/1023;
      ADCFlag=0; 
      ADMUX = 0x41;   // PortF2 아날로그 입력 선택 
   }
   else ADCFlag=0;
}

void initPort(void)
{
   DDRC  = 0xff;   // LCD 데이터 및 명령 
   PORTC = 0x00;
   DDRG  = 0x0f;   // LCD 제어 출력 (RS, RW, E)
   DDRF  = 0x00;   // 아날로그 입력 

   DDRE  = 0x00;
}



int main(void)
{
   initPort();      // 입출력 포트 초기화
    LCD_init();     // LCD 초기화
   can_init(b250k);   // CAN 보레이트를 원하는 값으로 세팅한다.
   initAdc();      // AD 변환 초기화
   sei();         // INT 인에이블 

   while(1)
   {

      
      /********************************************
      실험1 소스코드
      ADcLCD()함수 이용
      msg1.data[]배열 이용 
       can_tx() 함수 이용
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
