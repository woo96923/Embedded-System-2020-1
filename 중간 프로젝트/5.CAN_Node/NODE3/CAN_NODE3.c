#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>         

#include "myDelay.h"
#include "myDelay.c"

#include "lcdControl.h"
#include "myCANLib.h"
#include "myCANLib.c"


///////////////////////////////////////////////////////////////
#define TIMSK _SFR_I08(0x37)
#define ON 0
#define OFF 1
#define DO 0
#define RE 1
#define MI 2
#define FA 3
#define SOL 4
#define RA 0
#define SI 0
#define HDO 0

#define EOS -1

volatile int state, tone;
char f_table[8] = {17,43,66,77,97,114,117,137};

//int song[] = {SOL, MI, MI, SOL, MI, DO, RE, MI, RE, DO, MI, SOL, HDO, SOL, HDO, SOL, HDO, SOL, MI, SOL, RE, FA, MI, RE, DO, EOS}; 

int error[] = {SOL, SOL, SOL, EOS};

#define F_CPU 16000000UL

ISR(TIMER0_OVF_vect)
{
	if(state == OFF)
	{
	//PORTE |= 1;
	state = ON;
	}
	else
	{
	//PORTE &= ~1;
	//PORTE&=0xFC;
	state = OFF;
	}
	TCNT0 = f_table[tone];
}

//////////////////////////////////////////////////////////////

#define FREQ_CLKIO      16000000   // clock IO 16 MHz
#define PRESCALE      64
#define RC_SERVO_PERIOD   25         // DC ���� �ֱ� 25 msec

unsigned int ADdata;

struct MOb msg2; //���� �� �ִ¾�, 1���� �������� ��, ���� ���, ���� �ӵ� ����

//struct MOb msg3={0x01, 0, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};  ;//���� �� �޴¾� 

struct MOb msg4={0x01, 0, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};  ;//���� �� �޴¾� 


/////////cds control/////////////
unsigned int cds=0;
unsigned int ADdata;



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
//���� ��� 
//unsigned char array[10][10]={"OOOOOOOOO", "OOOOOOOO ", "OOOOOOO  ", "OOOOOO   ", "OOOOO    ", "OOOO     ", "OOO      ", "OO       ", "         "};
unsigned int close=0;
unsigned int angle_check;
unsigned int array[11][2]={90,80,70,60,50,40,30,20,10,0};

unsigned int *ArraySelect(void)
{
	close =0;
	if(ADdata>1000) return array[0][0];
	else if(ADdata>950) return array[1][0];
	else if(ADdata>900) return array[2][0];
	else if(ADdata>850) return array[3][0];
	else if(ADdata>800) return array[4][0];
	else if(ADdata>750) return array[5][0];
	else if(ADdata>700) return array[6][0];
	else if(ADdata>650) return array[7][0];
	else if(ADdata>600) return array[8][0];
	else if(ADdata>500) {close =1; return array[9][0];}
	return array[10][0];
/*	if(ADdata>300) return array[0][0];
	else if(ADdata>298) return array[1][0];
	else if(ADdata>295) return array[2][0];
	else if(ADdata>293) return array[3][0];
	else if(ADdata>290) return array[4][0];
	else if(ADdata>288) return array[5][0];
	else if(ADdata>285) return array[6][0];
	else if(ADdata>282) return array[7][0];
	else if(ADdata>279) return array[8][0];
	else if(ADdata>277) {close =1; return array[9][0];}
	return array[10][0];*/
}
/////////////////////////////////


void initPort(void)
{
   DDRC  = 0xff;   // LCD ������ �� ��� 
   PORTC = 0x00;
   DDRG  = 0x0f;   // LCD ���� ��� (RS, RW, E)
   DDRE  = 0xff;   // Motor�� �̿��ϱ� ���� ��Ʈ. �ʿ��� �� ��� ����

///////+cds control+///////
	DDRF  = 0x00;	
/////////////////////////

}

// ���� �ʱ�ȭ
void initMotor(void)
{
   TCCR3A=   0b10001010;   // COM3A[1:0]=10,  ����ġ�� Ŭ����
            //               TOP �� 
            // WGM3[3:0] :     Phase correct PWM mode 
            //                 TOP���� ICR �������Ͱ� ����
   TCCR3B=   0b11010011;   // 64 ���� 
               
   ICR3 = FREQ_CLKIO/2/PRESCALE/1000*RC_SERVO_PERIOD; // �ְ�(Top)3125, 40Hz(25msec) 
   OCR3C= 70;   
}

void CtrlRcServoMotor(unsigned int degree)
{
	PORTE&=0xFC;
    OCR3C= FREQ_CLKIO/2/PRESCALE/1000*(1.6/180*degree+(0.15/180*degree+0.6));
}


// ���� ���� ȸ���� ���� 
unsigned int RcServoMotorAngle(unsigned int volt_servo)
{
   unsigned int angle;
   angle = (volt_servo)*90/50;
   return angle;
}


int main(void)
{
   unsigned int current_speed;
   unsigned int current_mode;
   unsigned int tempVoltage=0;
   unsigned int donotPlay;

   unsigned int warn=0;
   unsigned int angle;
// unsigned int tik;
   initPort();      // ����� ��Ʈ �ʱ�ȭ
   LCD_init();     // LCD �ʱ�ȭ
   initMotor();   // ���ܸ��� ��� ���� Ÿ�̸�/ī���� �ʱ�ȭ 

/////////cds control/////////
   initAdc();		// AD ��ȯ �ʱ�ȭ   
   sei();           // INT �ο��̺� 
////////////////////////////

//ȭ�� �ʱ�ȭ!
    LCD_cmd(0x01);	// LCD ȭ�� ����� 
	ms_delay(50);	// 50ms ���� 

	LCD_Write(0,0, "regi");	// �ʱ� ���� ���
	ms_delay(200);
	
	ms_delay(200);
///////////////////////

    can_init(5); 		// �ʱ�ȭ
	can_init_8Mhz(5);
	can_rx_set(2, 0x02, EXT, 8, 0x00, 0x05  ); 	

   while(1)
   {       
      //ĵ data �޾ƿ��� 
      can_rx(2, &msg2);
	  current_mode = (int)msg2.data[0];
	  current_speed = (int)msg2.data[2];
	  int ting = (int)msg2.data[4];
	  if(ting != 0 ) tempVoltage = ting;
	  //�������� ��� 
	  

	 if(ADdata <700 ){
	  	warn = 1;
	  }
	  else warn = 0;
	  
	  if(/*current_speed == 0 && */current_mode ==0)
	  {
	    
	    angle = RcServoMotorAngle(tempVoltage);//0~50 �������� �� 0~90������ ��ȯ 
	//	if(tik)
		CtrlRcServoMotor(angle);
		ms_delay(50);
	//	tik=0
	 }
	  unsigned char tempV[4];
 	  sprintf(tempV, "%d", angle); 
	  LCD_Write(5,0,tempV);

	  ///////// jodo control ////////////
		//�ݾҴµ� ���� ������ ����(1)�� 
	  if( (angle > 75) && (warn != 1)) {
	  
	  donotPlay=1;
	  LCD_Write(0,1, "WARNING");	// �ʱ� ���� ���
	  unsigned int temp1[2];
	  sprintf(temp1,"%d", donotPlay);
	  LCD_Write(8,1,temp1);
	  
	  }
	  else {donotPlay = 0;
	  LCD_Write(0,1, "            ");	// �ʱ� ���� ���
	  }
	  /*if(donotPlay == 1){
	  
		    int buz = 0;
			DDRE  = 0x01;
			TCCR0A = 0x03;
			TIMSK0 = 0x01;
			TCNT0 = f_table[error[buz]];
			sei();
			do {
			tone = error[buz++];
			ms_delay(100);
			}while(tone != EOS);//the end	 
			//DDRE &= 0x00;  
	  }*/
	  

	  
	  //unsigned int printAngle[3]; 
	  //printAngle[2] = 
	 // sprintf(printAngle, "%d", ArraySelect());
	  
	  //LCD_Write(5,1,printAngle);
	  unsigned int temp[2];
	  sprintf(temp,"%d", warn);
	  LCD_Write(8,0,temp);//������ �ݾҴ�. 
	  



	  ////CAN!!! �۽� ///
	  //msg4.data[0] = (char)ArraySelect();
	  //msg4.data[2] = (char)donotPlay;
//  if (current_mode == 0 ){
      //   can_tx(4, &msg4, 0);

	  //}
////////////////////////////////////////
      
}
}


