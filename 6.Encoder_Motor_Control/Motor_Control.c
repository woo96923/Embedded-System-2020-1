/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project3
edit : wwChoi
---------------------------------------------------------------------------------------------

 Experiment1 -> ��ư�� �̿��Ͽ� ���͸� �����ϰ�, ���ڴ��� ���� �ǵ���� �޾� LCD�� ��갪�� RPM�������� ǥ���Ѵ�.

--------------------------------------------------------------------------------------------*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>			
#include "lcdControl.h"



#define FREQ_CLKIO		16000000	// clock IO 16 MHz
#define PRESCALE		64
#define DC_PERIOD		25			// DC ���� �ֱ� 25 msec

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
	DDRC  = 0xff;   // LCD ������ �� ��� 
	DDRG  = 0b111;   // LCD ���� ��� (RS, RW, E)
 	DDRD  = 0x00;   // SW �Է¼���
   	DDRE  = 0x0f;   // Motor�� �̿��ϱ� ���� ��Ʈ. �ʿ��� �� ��� ����


}

// DC���� �ʱ�ȭ
void initMotor(void)
{
	TCCR3A= 0x82;
	TCCR3B=	0x13;		 
					 
	ICR3 = FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD;	
					// �ְ�(Top)3125, 40Hz(25msec) 
}

// DC���� �ӵ� ����� ���� ȸ�� ���� ���� 
void CtrlDcMotor(unsigned int speed, unsigned int dir)
{
	unsigned int level=5;

	PORTE&=0xFC;

	// DC���� ȸ�� ���� ����
	if(dir==0)		// �ð� ���� ȸ��
	{
		// speed�� ���� �ӵ� ����
		OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD)/level;
		
	}
	else if(dir==1)	// �ݽð� ���� ȸ�� 
	{
		// speed�� ���� �ӵ� ����
		OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD)/level;
	}
	else			// ���� ���� 
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

	if(in>0) up=0;	// S2��ư �� ������ 
	else up=1;		// S2��ư ������

	// ����ũ�� �̿��� ��ƮD �Է¿��� 1���� �Է� ���� 
	in = PIND & BUTTON1_MASK;

	if(in>0) down=0;	// S3��ư �� ������	
	else down=1;		// S3��ư ������

	if(up==1 && down==0) return SPEED_UP;	// S2��ư ���� ���
	if(up==0 && down==1) return SPEED_DOWN;	// S3��ư ���� ���

	return SPEED_NO_CAHNGE;
}


// ���� �ӵ��� ���� �Լ� (��ư �Է¿� ����)
unsigned char DcMotorSpeedControl(unsigned char bt_in, unsigned char speed)
{
	// S2 ��ư�� �������� 
	if(bt_in==SPEED_UP) 
	{
		if(speed==SPEED_MAX) return speed;
		return (++speed);	// �ӵ� 1 ���� 
	}
	// S3 ��ư�� ��������
	if(bt_in==SPEED_DOWN)
	{
		if(speed==SPEED_MIN) return speed;
		return (--speed);	// �ӵ� 1 ���� 
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
	//external interrupt��� ��������
	EIMSK = 0x88;	// ����Ʈ���� INT7 INT6 INT5 INT4 INT3 INT2 INT1 INT0 ���� ���ͷ�Ʈ ����Ʈ1, ���X��Ʈ0
	EICRB = 0xc0;    // EICRA =>  INT0 INT1 INT2 INT3������ ���ͷ�Ʈ ��¿��� �� �ϰ����� ���� ��,  EICRB  INT7 INT6 INT5 INT4 ������ ��
	SREG =  0x80;    // �� ��Ʈ7�� ���ͷ�Ʈ ��뿡 ���� ��
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
	initPort();				// ����� ��Ʈ �ʱ�ȭ
   	LCD_init();     		// LCD �ʱ�ȭ
	external_intr_init(); 	//�ܺ����ͷ�Ʈ ���� ��������
	initMotor();	// ���ܸ��� ��� ���� Ÿ�̸�/ī���� �ʱ�ȭ 
	sei();			// INT �ο��̺� 

	LCD_cmd(0x01);	// LCD ȭ�� ����� 
	ms_delay(50);	// 50ms ���� 

	LCD_Write(0,0, table1);	// �ʱ� ���� ���
	LCD_Write(0,1, table2);
	ms_delay(1000);

	LCD_cmd(0x01);			// LCD ȭ�� ����� 
	ms_delay(100);
 
 	unsigned char *button;
    unsigned char *state;
    unsigned int button_check=0;
	unsigned int sec=0;
	while(1)
	{	
	/**************************************************
	����
	: �������� �Լ��� �̿��Ͽ� �ҽ��ڵ� ����
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
		//speed = DcMotorSpeedControl(out,speed); //��ư �Է�, �ӵ��� ���ؼ� �ٲ�  
		CtrlDcMotor(speed,0);

		sprintf(temp,"%d",speed);
        LCD_Write(13,0, temp);
		
	}

}
