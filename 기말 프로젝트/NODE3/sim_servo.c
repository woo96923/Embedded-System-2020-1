#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>         

#include "myDelay.h"

#include "servo.h"                     /* Model's header file */
#include "rtwtypes.h"                  /* MathWorks types */
#include "servo_private.h"
#include "servo_types.h"

#include "lcdControl.h"
#include "myCANLib.h"

unsigned int current_speed;
unsigned int current_mode;
unsigned int ADdata;
unsigned int ting;

unsigned int warn_stop;
unsigned int cds=0;
unsigned int angle;

struct MOb msg2; 

#define FREQ_CLKIO      16000000   // clock IO 16 MHz
#define PRESCALE      64
#define RC_SERVO_PERIOD   25         // DC ���� �ֱ� 25 msec

///////////////buzzer/////////////////////////
#define TIMSK _SFR_I08(0x37)
#define ON 0
#define OFF 1
#define DO 0
#define RE 1
#define MI 2
#define FA 3
#define SOL 4
#define RA 5
#define SI 6
#define HDO 7

#define EOS -1

volatile int state, tone;
char f_table[8] = {17,43,66,77,97,114,117,137};
int err[] = {DO,RE,MI,EOS};

#define F_CPU 16000000UL

ISR(TIMER0_OVF_vect)
{
	if(state == OFF)
	{
	PORTE |= 1;
	state = ON;
	}
	else
	{
	PORTE &= ~1;
	state = OFF;
	}
	TCNT0 = f_table[tone];
}
///////////////////////////////////////////////


void rt_OneStep(void);
void rt_OneStep(void)
{
  static boolean_T OverrunFlag = 0;

  if (OverrunFlag) {
    rtmSetErrorStatus(servo_M, "Overrun");
    return;
  }

  OverrunFlag = TRUE;

  servo_U.current_mode = current_mode;
  servo_U.current_speed = current_speed;
  servo_U.ting = ting;
  servo_U.ADdata = ADdata;


  servo_step();

  warn_stop = servo_Y.warn_stop;
  angle = (int)servo_Y.angle;
  cds = (int)servo_Y.cds;
  OCR3C = servo_Y.ocr3c;
  PORTE= servo_Y.porte;

  OverrunFlag = FALSE;

}


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
	ADdata= ADC;				// AD��ȯ �����͸� ADdata �� ���� ( Register�� �ִ°��� �������� ADdata�� �Ű� ����ϱ� ���� �̿��Ѵ�. )
   	ADCSRA= 0b11001000;  	    // ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺� 
								// ADC Ŭ�� ����; XTAL��1/2(8MHz)
}

void initPort(void)
{
   DDRC  = 0xff;   // LCD ������ �� ��� 
   PORTC = 0x00;
   DDRG  = 0x0f;   // LCD ���� ��� (RS, RW, E)

   DDRE  = 0xff;   // Motor�� �̿��ϱ� ���� ��Ʈ. �ʿ��� �� ��� ����
   DDRB |= 0b10000000;//module �� led ����

///////+cds control+///////
   DDRF  = 0x00;	
///////////////////////////


}

// ���� �ʱ�ȭ
void initMotor(void)
{
   TCCR3A=   0b10001010;   // COM3A[1:0]=10,  ����ġ�� Ŭ���� WGM3[3:0]: Phase correct PWM mode 
   TCCR3B=   0b11010011;   // 64 ���� 

   ICR3 = FREQ_CLKIO/2/PRESCALE/1000*RC_SERVO_PERIOD; // �ְ�(Top)3125, 40Hz(25msec) 
   OCR3C= 70;   
}

int main(void)
{
   initPort();      // ����� ��Ʈ �ʱ�ȭ
   LCD_init();     // LCD �ʱ�ȭ
   initMotor();   // ���ܸ��� ��� ���� Ÿ�̸�/ī���� �ʱ�ȭ 

//cds control
   initAdc();		// AD ��ȯ �ʱ�ȭ   
   sei();           // INT �ο��̺� 


//ȭ�� �ʱ�ȭ
    LCD_cmd(0x01);	 
	ms_delay(50);	 

	LCD_Write(0,0, "bar");	
	ms_delay(20);

    can_init(5); 		
	can_init_8Mhz(5);
	can_rx_set(2, 0x02, EXT, 8, 0x00, 0x05  ); 	

   int flag=0;
   int check=1;
   int start = 1;
   int diff = 0;

   while(1)
   {       
      can_rx(2, &msg2);
	  current_mode = (int)msg2.data[0];
	  current_speed = (int)msg2.data[2];
	  ting = (int)msg2.data[4];
	  if((int)msg2.data[4]>=10){ting = (int)msg2.data[4];}
      
	  rt_OneStep();
	  

	  if((int)msg2.data[4]>=10){
	  unsigned char tempV[4];
 	  sprintf(tempV, "%d", ting-10); 
	  LCD_Write(5,0,tempV);
	  }
	  unsigned char tempVV[3];
 	  sprintf(tempVV, "%d", cds); 
	  LCD_Write(8,0,tempVV);

	  unsigned char tempVVV[4];
 	  sprintf(tempVVV, "%d", ADdata); 
	  LCD_Write(8,1,tempVVV);


	  unsigned int temp1[2];
	  sprintf(temp1,"%d", warn_stop);
	  LCD_Write(11,0,temp1);

	  sei();

	  if (warn_stop ==1 && flag >0){
	  	flag--;
	  }
	  else if( warn_stop == 1 && flag ==0 && check == 1) {
	  	check=0;
	  	flag=50;
	  	LCD_Write(0,1, "WARNING");	// �ʱ� ���� ���
	  
	    //////buzzer////////
        int buz = 0;
		DDRE  |= 0x01;
		TCCR0A = 0x03;
	    TIMSK0 = 0x01;
		TCNT0 = f_table[err[buz]];
			
		tone = err[buz++];
		while(tone != EOS) {
			ms_delay(10);
			tone = err[buz++];
		}
		DDRE = 0xfe;
		//////////////////

	   	PORTB=0b00000000;//Module �� LED On
	  }
	  else {
	  	check=1;
	  	LCD_Write(0,1, "            ");	// �ʱ� ���� ���
	  	PORTB=0b10000000;
	  }
   }
}


