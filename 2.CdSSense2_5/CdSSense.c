/*------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project2
edit : wwChoi
--------------------------------------------------------------
 Experiment1 -> LCD를 이용하여 Time값을 증가하게 표시
 Experiment2 -> ADC를 이용한 CDS값 LCD에 표시 
 Experiment3 -> ADC를 이용한 TEMP값 LCD에 표시 
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
// AD 변환 
//=========================================================
void initAdc(void)
{
	ADMUX = 0x40 ;   // 기준전압; 외부 캐퍼시터 가진 AVcc(AREF 핀)
				// AD변환 데이터 정렬; 오른쪽 정렬 
				// AD변환 채널 선택; PortF 0 핀	 

	DDRF = 0xf0;	// PortF[3..0] 입력으로 설정, PortF[7..4] 출력으로 설정 
	DIDR0 = 0x0f;	// 디지털 입력 불가 PortF[3..0]

	ADCSRA = 0xc8;  	// ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블 
					// ADC 클럭 설정; XTAL의1/2(8MHz) //11001000	  	
}

SIGNAL(ADC_vect)
{
	ADdata= ADC;	// AD변환 데이터를 ADdata 에 저장 ( Register에 있는값을 전역변수 ADdata에 옮겨 사용하기 쉽게 이용한다. )
   	ADCSRA= 0b11001000;  	    // ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블 
					// ADC 클럭 설정; XTAL의1/2(8MHz)
}

// 입출력 초기화 
void initPort(void)
{
	DDRC  = 0xff;	// LCD 데이터 및 명령 
	PORTC = 0x00;
	DDRG  = 0x0f;	// LCD 제어 출력 (RS, RW, E)
	DDRF  = 0x00;	

	DDRE  = 0x00;
}

// CdS센서의 AD변환 값에 따라서 array 배열의 행 주소 리턴  
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
	
	initPort();		// 입출력 포트 초기화
    LCD_init();     // LCD 초기화

	LCD_cmd(0x01);	// LCD 화면 지우기 
	ms_delay(50);	// 50ms 지연 

	LCD_Write(0,0, table1);	// 초기 글자 출력
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
		 실험1 코드작성
		******************************************/
		
	}

#elif Experiment == 2
	unsigned char  *ptr;	

	initPort();		// 입출력 포트 초기화
    LCD_init();     // LCD 초기화
	initAdc();		// AD 변환 초기화

	sei();			// INT 인에이블 : 3장에서 배울예정. 인터럽트를 사용하기 위하여 전체인터럽트를 허용해주는 함수이다.

	LCD_cmd(0x01);	// LCD 화면 지우기 
	ms_delay(50);	// 50ms 지연 

	LCD_Write(0,0, table1);	// 초기 글자 출력
	LCD_Write(0,1, table2);
	ms_delay(200);

	//LCD_cmd(0x01);	// LCD 화면 지우기 
	ms_delay(100);

	while(1)
	{	
		LCD_Write(4,1,ArraySelect());
		ms_delay(100);
		/*****************************************
		 실험2 ArraySelect 함수를 이용하여 코드작성
		******************************************/
	}

#elif Experiment == 3     //힌트 : 실험2와 같은 방식으로 구현한다.
	unsigned char temp;
	unsigned char  *ptr;	

	initPort();		// 입출력 포트 초기화
    LCD_init();     // LCD 초기화
	initAdc();		// AD 변환 초기화

	sei();			// INT 인에이블 : 3장에서 배울예정. 인터럽트를 사용하기 위하여 전체인터럽트를 허용해주는 함수이다.

	LCD_cmd(0x01);	// LCD 화면 지우기 
	ms_delay(50);	// 50ms 지연 

	LCD_Write(0,0, table1);	// 초기 글자 출력
	LCD_Write(0,1, table2);
	ms_delay(200);

	//LCD_cmd(0x01);	// LCD 화면 지우기 
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
		 실험3 ADC2Temp() 함수를 이용하여 코드작성
		******************************************/
	}

#endif 

}

