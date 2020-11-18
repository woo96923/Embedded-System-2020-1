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
#include <stdio.h>			// sprintf�� ����Ǿ� ���� 

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
	DIDR0 = 0x07;	// ������ �Է� �Ұ� PortF[3..0]
   	ADCSRA= 0xC8;	  	

}
void initAdc_Temp(void)
{
   	ADMUX = 0x01;	    
	DDRF  = 0x00;		
	DIDR0 = 0x07;	// ������ �Է� �Ұ� PortF[3..0]
   	ADCSRA= 0xC8;	  	

}
SIGNAL(ADC_vect)
{
	ADdata= ADC;	// AD��ȯ �����͸� ADdata �� ���� ( Register�� �ִ°��� �������� ADdata�� �Ű� ����ϱ� ���� �̿��Ѵ�. )
   	ADCSRA= 0xC8;   	    // ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺� 
					// ADC Ŭ�� ����; XTAL��1/2(8MHz)
}
void initPort(void)
{
	DDRC  = 0xff; //LCD ������ �� ���
	PORTC = 0x00;   //LCD�� ����Ǿ� �����͸� �������Ƿ� ��� ����
	DDRG  = 0xff; //LCD ���� ���
	DDRF  = 0x00;     //�Է¹�������... �ؼ� 00�־
	DDRE  = 0x00; //���͸� �̿��ϱ� ���� ��Ʈ
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
	initPort();		// ����� ��Ʈ �ʱ�ȭ

    LCD_init();     // LCD �ʱ�ȭ

	can_init(5); 		// �ʱ�ȭ
	can_init_8Mhz(5);
	can_rx_set(0, 0x01, EXT, 8, 0x00, 0  ); 	// CAN ���ű� �ʱ�ȭ 
	
	sei();			// INT �ο��̺� 

	LCD_cmd(0x01);	// LCD ȭ�� ����� 
	
	while(1)
	{
		/*idx�� ¦���̸� cds�� ������ flag0����*/
		if(idx % 2 == 0)
		{
			initAdc_CDS();
			msg2.data[0] = ADdata;
			msg2.data[1] = '0';
		}
		/*idx�� Ȧ���̸� temp�� ������ flag1����*/		
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
		/*MCU4->MCU1temp,cds���� ����*/
		can_tx(0,&msg2,0);
		/*MCU1->MCU4*/
		can_rx(3, &msg1);
		/*2�̸� ���� ���� ���õ�,3�̸� �ĸ� ����*/
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
