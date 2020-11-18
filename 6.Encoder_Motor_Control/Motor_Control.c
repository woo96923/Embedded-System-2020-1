/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project3
edit : wwChoi
---------------------------------------------------------------------------------------------

 Experiment1 -> 버튼을 이용하여 모터를 구동하고, 엔코더에 대한 피드백을 받아 LCD에 계산값을 RPM형식으로 표시한다.

--------------------------------------------------------------------------------------------*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>			
#include "lcdControl.h"



#define FREQ_CLKIO		16000000	// clock IO 16 MHz
#define PRESCALE		64
#define DC_PERIOD		25			// DC 모터 주기 25 msec

#define BUTTON0_MASK 0x01
#define BUTTON1_MASK 0x02

#define SPEED_NO_CAHNGE	0
#define SPEED_UP 		1
#define SPEED_DOWN 		2
#define SPEED_MAX		5
#define SPEED_MIN		0


unsigned char table1[16]="AJOU-UNIVERSITY";
unsigned char table2[16]="SPEED_CONTROL";
unsigned char strBuff[20]={0};

unsigned int rpm_time_check_count =0, RPM =0, speed_count =0 ;

void initTimer(void){

	TCCR0A = 0b00001010; //(1<<WGM01);
	//TCCR0A |= (1<<CS01);// clk/8 Prescaler
	TCNT0 = 0;
	OCR0A = 19; //1cycle->20us = 1/(16Mhz/(2*8*(19+1)))
	TIMSK0 = (1<<OCIE0A);

}

SIGNAL(TIMER0_COMP_vect){
	speed_count++;
}

void initPort(void)
{
	DDRC  = 0xff;   // LCD 데이터 및 명령 
	DDRG  = 0b111;   // LCD 제어 출력 (RS, RW, E)
 	DDRD  = 0x00;   // SW 입력설정
   	DDRE  = 0x0f;   // Motor를 이용하기 위한 포트. 필요한 핀 출력 설정


}

// DC모터 초기화
void initMotor(void)
{
	TCCR3A= 0x82;
	TCCR3B=	0x13;		 
					 
	ICR3 = FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD;	
					// 최고값(Top)3125, 40Hz(25msec) 
}

// DC모터 속도 제어와 모터 회전 방향 제어 
void CtrlDcMotor(unsigned int speed, unsigned int dir)
{
	unsigned int level=5;

	PORTE&=0xFC;

	// DC모터 회전 방향 결정
	if(dir==0)		// 시계 방향 회전
	{
		// speed에 따른 속도 제어
		OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD)/level;
		
	}
	else if(dir==1)	// 반시계 방향 회전 
	{
		// speed에 따른 속도 제어
		OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD)/level;
	}
	else			// 모터 정지 
	{
		OCR3A=100;
		OCR3B=100;
	}
}



// 포터D의 0번(S3)과 1번(S2) 핀에서 입력 받음 
unsigned char ButtonInput(void)
{
	unsigned char in;
	unsigned char up;
	unsigned char down;
		
	// 마스크를 이용해 포트D 입력에서 0번핀 입력 추출 
	in = PIND & BUTTON0_MASK;

	if(in>0) up=0;	// S2버튼 안 눌러짐 
	else up=1;		// S2버튼 눌러짐

	// 마스크를 이용해 포트D 입력에서 1번핀 입력 추출 
	in = PIND & BUTTON1_MASK;

	if(in>0) down=0;	// S3버튼 안 눌러짐	
	else down=1;		// S3버튼 눌러짐

	if(up==1 && down==0) return SPEED_UP;	// S2버튼 누른 경우
	if(up==0 && down==1) return SPEED_DOWN;	// S3버튼 누른 경우

	return SPEED_NO_CAHNGE;
}


// 모터 속도값 제어 함수 (버튼 입력에 따른)
unsigned char DcMotorSpeedControl(unsigned char bt_in, unsigned char speed)
{
	// S2 버튼이 눌러지면 
	if(bt_in==SPEED_UP) 
	{
		if(speed==SPEED_MAX) return speed;
		return (++speed);	// 속도 1 증가 
	}
	// S3 버튼이 눌러지면
	if(bt_in==SPEED_DOWN)
	{
		if(speed==SPEED_MIN) return speed;
		return (--speed);	// 속도 1 감소 
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


void external_intr_init(void)
{
	//external interrupt허용 레지스터
	EIMSK = 0x88;	// 각비트별로 INT7 INT6 INT5 INT4 INT3 INT2 INT1 INT0 순서 인터럽트 허용비트1, 허용X비트0
	EICRB = 0xc0;    // EICRA =>  INT0 INT1 INT2 INT3에대한 인터럽트 상승엣지 및 하강엣지 관한 것,  EICRB  INT7 INT6 INT5 INT4 에관한 것
	SREG =  0x80;    // 맨 비트7은 인터럽트 허용에 관한 것
}


ISR(INT7_vect) {
	rpm_time_check_count++;
}



int main(void)
{
 	unsigned int speed=0;
	unsigned int out=0;
	char temp[7];

	initTimer();
	initPort();				// 입출력 포트 초기화
   	LCD_init();     		// LCD 초기화
	external_intr_init(); 	//외부인터럽트 세팅 레지스터
	initMotor();	// 스텝모터 제어를 위한 타이머/카운터 초기화 
	sei();			// INT 인에이블 

	LCD_cmd(0x01);	// LCD 화면 지우기 
	ms_delay(50);	// 50ms 지연 

	LCD_Write(0,0, table1);	// 초기 글자 출력
	LCD_Write(0,1, table2);
	ms_delay(1000);

	LCD_cmd(0x01);			// LCD 화면 지우기 
	ms_delay(100);
 
 	unsigned char *button;
    unsigned char *state;
    unsigned int button_check=0;
	unsigned int sec=0;
	while(1)
	{	
	/**************************************************
	실험
	: 모터제어 함수를 이용하여 소스코드 구현
	**************************************************/
	if(speed_count > 50000){
			RPM = 60*rpm_time_check_count/0.5/30/13;
			sprintf(temp,"%d",RPM);
			LCD_Write(0,1,temp);
			sec++;
			rpm_time_check_count = 0;			
			speed_count = 0;
		}

	//us_delay(20);
	//speed_count++;

	sprintf(temp,"%d",speed_count);
	LCD_Write(0,0,temp);

	sprintf(temp,"%d",sec);
	LCD_Write(7,0,temp);
	

		button = ButtonInput();

	   if (button != state) button_check = 0;

	   if (button_check == 0)
	   {
	      speed = DcMotorSpeedControl(button, speed);
	      button_check = 1;
		  state = button;
	   }

		//out = ButtonInput();
		//speed = DcMotorSpeedControl(out,speed); //버튼 입력, 속도에 의해서 바뀜  
		CtrlDcMotor(speed,0);

		sprintf(temp,"%d",speed);
        LCD_Write(13,0, temp);
		
	}

}
