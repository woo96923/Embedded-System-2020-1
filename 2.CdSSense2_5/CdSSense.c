/*------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project2
edit : wwChoi
--------------------------------------------------------------
 Experiment1 -> LCD�� �̿��Ͽ� Time���� �����ϰ� ǥ��
 Experiment2 -> ADC�� �̿��� CDS�� LCD�� ǥ�� 
 Experiment3 -> ADC�� �̿��� TEMP�� LCD�� ǥ�� 
------------------------------------------------------------*/

#define Experiment 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>	
	
#include "delay.h"
#include "lcdControl.h"


#if Experiment == 1
char table1[16]="LCD DISPLAY";
char table2[16]="Time: ";

#elif Experiment == 2
char table1[16]="Ajou University";
char table2[16]="CdS Sense";

#elif Experiment == 3
char table1[16]="Ajou University";
char table2[16]="Temp Sense";
#endif

unsigned char array[10][10]={"OOOOOOOOO",
							 "OOOOOOOO ",
							 "OOOOOOO  ",
							 "OOOOOO   ",
							 "OOOOO    ",
							 "OOOO     ",
							 "OOO      ",
							 "OO       ",
							 "         "};
char strBuff[20]={0};							 


unsigned int ADdata; 
unsigned int cds=0;




//=========================================================
// AD ��ȯ 
//=========================================================
void initAdc(void)
{
	ADMUX = 0x40 ;   // ��������; �ܺ� ĳ�۽��� ���� AVcc(AREF ��)
				// AD��ȯ ������ ����; ������ ���� 
				// AD��ȯ ä�� ����; PortF 0 ��	 

	DDRF = 0xf0;	// PortF[3..0] �Է����� ����, PortF[7..4] ������� ���� 
	DIDR0 = 0x0f;	// ������ �Է� �Ұ� PortF[3..0]

	ADCSRA = 0xc8;  	// ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺� 
					// ADC Ŭ�� ����; XTAL��1/2(8MHz) //11001000	  	
}

SIGNAL(ADC_vect)
{
	ADdata= ADC;	// AD��ȯ �����͸� ADdata �� ���� ( Register�� �ִ°��� �������� ADdata�� �Ű� ����ϱ� ���� �̿��Ѵ�. )
   	ADCSRA= 0b11001000;  	    // ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺� 
					// ADC Ŭ�� ����; XTAL��1/2(8MHz)
}

// ����� �ʱ�ȭ 
void initPort(void)
{
	DDRC  = 0xff;	// LCD ������ �� ��� 
	PORTC = 0x00;
	DDRG  = 0x0f;	// LCD ���� ��� (RS, RW, E)
	DDRF  = 0x00;	

	DDRE  = 0x00;
}

// CdS������ AD��ȯ ���� ���� array �迭�� �� �ּ� ����  
unsigned char *ArraySelect(void)
{

	if(ADdata>1000) return array[0];
	else if(ADdata>950) return array[1];
	else if(ADdata>900) return array[2];
	else if(ADdata>850) return array[3];
	else if(ADdata>800) return array[4];
	else if(ADdata>750) return array[5];
	else if(ADdata>700) return array[6];
	else if(ADdata>650) return array[7];
	else if(ADdata>600) return array[8];

	return array[9];
}



unsigned int ADC2Temp(int get_ADC)
{
	unsigned int ADC_temp;
	ADC_temp = get_ADC/20;

	return ADC_temp;
}

int main(void)
{
#if Experiment == 1
	unsigned int  time=0, time_count =0;
	char time_char[6] = "00:00";	
	
	initPort();		// ����� ��Ʈ �ʱ�ȭ
    LCD_init();     // LCD �ʱ�ȭ

	LCD_cmd(0x01);	// LCD ȭ�� ����� 
	ms_delay(50);	// 50ms ���� 

	LCD_Write(0,0, table1);	// �ʱ� ���� ���
	LCD_Write(0,1, table2);
	ms_delay(200);
	
	
	while(1)
	{	
		time_char[4] = time_count+'0';
		if (time_count == 10)
		{
			time_char[3] = time_char[3] +1;
			if (time_char[3] == '6')
			{
				time_char[1] = time_char[1] +1;
				if (time_char[1] == '9'+1)
				{
					time_char[0] = time_char[0] +1;
					time_char[1] = '0';
				}
				time_char[3] = '0';
			}
			time_count = 0;
		}
		LCD_Write(5,1,time_char);
		time_count = time_count + 1;
		ms_delay(5);
		/*****************************************
		 ����1 �ڵ��ۼ�
		******************************************/
		
	}

#elif Experiment == 2
	unsigned char  *ptr;	

	initPort();		// ����� ��Ʈ �ʱ�ȭ
    LCD_init();     // LCD �ʱ�ȭ
	initAdc();		// AD ��ȯ �ʱ�ȭ

	sei();			// INT �ο��̺� : 3�忡�� ��￹��. ���ͷ�Ʈ�� ����ϱ� ���Ͽ� ��ü���ͷ�Ʈ�� ������ִ� �Լ��̴�.

	LCD_cmd(0x01);	// LCD ȭ�� ����� 
	ms_delay(50);	// 50ms ���� 

	LCD_Write(0,0, table1);	// �ʱ� ���� ���
	LCD_Write(0,1, table2);
	ms_delay(200);

	//LCD_cmd(0x01);	// LCD ȭ�� ����� 
	ms_delay(100);

	while(1)
	{	
		LCD_Write(4,1,ArraySelect());
		ms_delay(100);
		/*****************************************
		 ����2 ArraySelect �Լ��� �̿��Ͽ� �ڵ��ۼ�
		******************************************/
	}

#elif Experiment == 3     //��Ʈ : ����2�� ���� ������� �����Ѵ�.
	unsigned char temp;
	unsigned char  *ptr;	

	initPort();		// ����� ��Ʈ �ʱ�ȭ
    LCD_init();     // LCD �ʱ�ȭ
	initAdc();		// AD ��ȯ �ʱ�ȭ

	sei();			// INT �ο��̺� : 3�忡�� ��￹��. ���ͷ�Ʈ�� ����ϱ� ���Ͽ� ��ü���ͷ�Ʈ�� ������ִ� �Լ��̴�.

	LCD_cmd(0x01);	// LCD ȭ�� ����� 
	ms_delay(50);	// 50ms ���� 

	LCD_Write(0,0, table1);	// �ʱ� ���� ���
	LCD_Write(0,1, table2);
	ms_delay(200);

	//LCD_cmd(0x01);	// LCD ȭ�� ����� 
	ms_delay(100);
	char temp_char[3];
	while(1)
	{	temp = ADdata;
		sprintf(temp_char,"%d",ADC2Temp(ADdata));
		//temp_char[1] = ADC2Temp(temp)%10 + '0';
		//temp_char[0] = ADC2Temp(temp)/10 + '0';
		LCD_Write(11,1,temp_char);
		ms_delay(100);
		/*****************************************
		 ����3 ADC2Temp() �Լ��� �̿��Ͽ� �ڵ��ۼ�
		******************************************/
	}

#endif 

}

