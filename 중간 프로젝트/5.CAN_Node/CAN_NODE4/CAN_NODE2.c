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
#include <stdio.h>			// sprintf문 선언되어 있음 

#include "myDelay.h"
#include "lcdControl.h"
#include "myCANLib.h"

//#include "lcdControl.c"
//#include "myCANLib.c"

#define FREQ_CLKIO		16000000	// clock IO 16 MHz
#define PRESCALE		64


unsigned int ADdata; 
unsigned int cds=0;
struct MOb msg1;
struct MOb msg2={0x01, 0, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
char strBuff[20]={0};


void initAdc_CDS(void)
{
   	ADMUX = 0x00;	    
	DDRF  = 0x00;		
	DIDR0 = 0x07;	// 디지털 입력 불가 PortF[3..0]
   	ADCSRA= 0xC8;	  	

}
void initAdc_Temp(void)
{
   	ADMUX = 0x01;	    
	DDRF  = 0x00;		
	DIDR0 = 0x07;	// 디지털 입력 불가 PortF[3..0]
   	ADCSRA= 0xC8;	  	

}
SIGNAL(ADC_vect)
{
	ADdata= ADC;	// AD변환 데이터를 ADdata 에 저장 ( Register에 있는값을 전역변수 ADdata에 옮겨 사용하기 쉽게 이용한다. )
   	ADCSRA= 0xC8;   	    // ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블 
					// ADC 클럭 설정; XTAL의1/2(8MHz)
}
void initPort(void)
{
	DDRC  = 0xff; //LCD 데이터 및 명령
	PORTC = 0x00;   //LCD에 연결되어 데이터를 내보내므로 출력 설정
	DDRG  = 0xff; //LCD 제어 출력
	DDRF  = 0x00;     //입력받으려나... 해서 00넣어봄
	DDRE  = 0x00; //모터를 이용하기 위한 포트
}

unsigned int ADC2Temp(int get_ADC)
{
	unsigned int ADC_temp;
	ADC_temp = get_ADC/20;

	return ADC_temp;
}
void us_delay(unsigned int us_time)
{
	unsigned int i;

	for(i=0; i<us_time; i++) // 4 cycle +
	{
		asm("PUSH R0"); 	// 2 cycle +
		asm("POP R0"); 		// 2 cycle +
		asm("PUSH R0"); 	// 2 cycle +
		asm("POP R0"); 		// 2 cycle + =12 cycle for 11.0592MHZ
		asm("PUSH R0"); 	// 2 cycle +
		asm("POP R0"); 		// 2 cycle = 16 cycle = 1us for 16MHz
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
	unsigned int idx;
	unsigned char temp;
	int twink = 0;
	idx = 0;
	initPort();		// 입출력 포트 초기화

    LCD_init();     // LCD 초기화

	can_init(5); 		// 초기화
	can_init_8Mhz(5);
	can_rx_set(0, 0x01, EXT, 8, 0x00, 0  ); 	// CAN 수신기 초기화 
	
	sei();			// INT 인에이블 

	LCD_cmd(0x01);	// LCD 화면 지우기 
	
	while(1)
	{
		/*idx가 짝수이면 cds값 보내고 flag0전송*/
		if(idx % 2 == 0)
		{
			initAdc_CDS();
			msg2.data[0] = ADdata;
			msg2.data[1] = '0';
		}
		/*idx가 홀수이면 temp값 보내고 flag1전송*/		
		else
		{
			initAdc_Temp();
			temp = ADCTemp(ADdata);
			msg2.data[0] = temp;
			msg2.data[1] = '1';
		}
		msg2.data[2] = '\0';
		msg2.data[3] = '\0';
		msg2.data[4] = '\0';
		msg2.data[5] = '\0';
		msg2.data[6] = '\0';
		msg2.data[7] = '\0';
		/*MCU4->MCU1temp,cds정보 전송*/
		can_tx(0,&msg2,0);
		/*MCU1->MCU4*/
		can_rx(3, &msg1);
		/*2이면 우측 방향 지시등,3이면 후면 비상등*/
		if( ( msg1.data[6] == '2' ) || ( msg1.data[6] == '3')) {
			twink = 1;
		}
		else twink = 0;
		if(twink == 1) {
			PORTB = 0xff;
			ms_delay(300);
			PORTB = 0x00;
			ms_delay(300);
		}
		idx++;
	}
}
