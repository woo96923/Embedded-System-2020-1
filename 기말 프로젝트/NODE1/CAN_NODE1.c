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

#include "CAN_NODE1_AUTO.c"
#include "CAN_NODE1_AUTO.h"
#include "CAN_NODE1_AUTO_private.h"
#include "CAN_NODE1_AUTO_types.h"
#include "rtwtypes.h"

#define BUTTON0_MASK 0x01
#define BUTTON1_MASK 0x02
#define FREQ_CLKIO      16000000   // clock IO 16 MHz
#define PRESCALE      64
#define DC_PERIOD      25         // DC 모터 주기 25 msec

#define BUTTON0_MASK 0x01
#define BUTTON1_MASK 0x02

#define SPEED_NO_CHANGE   0
#define SPEED_UP       1
#define SPEED_DOWN       2
#define SPEED_MAX      5
#define SPEED_MIN      0

#define PE4_Clear   (PORTE &= 0xEF)
#define PE4_Set      (PORTE |= 0x10)
#define PE4_IN      (DDRE &= 0xEF)
#define PE4_OUT      (DDRE |= 0x10)

#define TEMPERATURE 25
unsigned short tick=0, pulse_check=0, pulse_end=0;
/*
#include "myDelay.c"

#include "myCANLib.c"
*/
unsigned int ADdata; 
unsigned int voltage; //_x, voltage_y;
unsigned int volume;//_x, volume_y;
unsigned char ADCFlag;
float distance2;
int distance_check;

struct MOb msg1 = {1, 0x01, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};    //to MCU2
struct MOb msg2 = {2, 0x02, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};   //to MCU3
struct MOb msg3;   //from MCU2
struct MOb msg4;	//from MCU3
/*   
struct MOb
{
   unsigned long id;
   unsigned char rtr;
   unsigned char ide;
   unsigned char dlc;
   unsigned char data[8];
};
*/
char strBuff[20]={0};
char mode[4][12] = { "WAIT", "ACCR", "SLOW", "WARN"};
int current_mode;
int current_speed;
int safeBarCheck = 0;



//=========================================================
// AD 변환 
// PF0: ADC0 조이스틱 y축 
// PF1: ADC1 조이스틱 x축
//=========================================================

void initAdc(void){
   ADMUX =  0x40;  // 기준전압; 외부 캐퍼시터 가진 AVcc(AREF 핀)
               // AD변환 데이터 정렬; 오른쪽 정렬 
               // AD변환 채널 선택; PortF 0 핀    

   DDRF  = 0xf0;   // PortF[3..0] 입력으로 설정, PortF[7..4] 출력으로 설정 
   DIDR0 = 0x0f;   // 디지털 입력 불가 PortF[3..0]

   ADCSRA= 0xc8;     // ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블 
               // ADC 클럭 설정; XTAL의1/2(8MHz)
}

void usart1_init(void)
{
	UCSR1A=	0x00; // serial states clear

	UCSR1B=	0x98; // 수신완료INT, 수신, 송신 인에이블 

	UCSR1C=	0x06; // stop bit; 0, no parity, 8bits set

	UBRR1H=	0x00; // 16MHz 9600bps set

	UBRR1L=	103;  // 16MHz 9600bps set
}

unsigned char usart1_receive( void )
{
	/* Wait for incomming data */
	while ( !(UCSR1A & (1<<RXC1)) );			                
	/* Return the data */
	return UDR1;
}

/**********************************************************
USART 통신 함수 제공. BUT 보고서에 원리 및 이해한 내용 첨부
***********************************************************/
void usart1_transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !(UCSR1A & (1<<UDRE1)) ); 			                
	/* Start transmittion */
	UDR1 = data; 			        
}

void usart1_transmit_string(char* p)
{
	while (*p)
		usart1_transmit(*p++);
}





SIGNAL(ADC_vect)
{
  ADdata = ADC;
  ADCSRA = 0xc8;
}

void ADcLCD(void)
{
      volume= ADdata; // 가변저항 PortF0
      voltage= volume*50/1023;      
   
}

void initPort(void)
{
   DDRC  = 0xff;   // LCD 데이터 및 명령 
   PORTC = 0x00;
   DDRG  = 0xff;   // LCD 제어 출력 (RS, RW, E)
   DDRF  = 0x00;   // 아날로그 입력 
   DDRD = 0x00;      //switch 입력 설정
   DDRE  = 0x00; 	//외부 switch 입력설정 

   DDRB = 0xff;
   PORTB = 0xff;
}
// ultra sound 
void init_EXT_INT(void)
{
   EIMSK |= (1<<INT4);      // INT4 Interrupt Set
   EICRB |= (1<<ISC41)|(1<<ISC40);      //INT4 Rising Edge /INT Set
   EIFR   = 1<<INT4;      // Interrupt Flag
}

void init_TIMER0_COMPA(void)
{
   TCCR0A |= (1<<WGM01);      // CTC Mode
   TCCR0A |= (1<<CS01);   // clk/8 Prescaler
   TCNT0 = 0;
   OCR0A = 19;      // 1cycle -> 20us으로 세팅 1cycle = 1/(16Mhz/(2*8*( OCR0A+1)))
   TIMSK0 = (1<<OCIE0A);      //T/C0 Output Compare Match INT
   TIFR0 = 0x00;
}

SIGNAL(TIMER0_COMP_vect)   
{
   tick++;
}

SIGNAL(INT4_vect)
{
   unsigned short pulse_tick;

   pulse_tick = tick;

   if(EICRB & 0x03)
   {
      EICRB &= 0x00;   //  low state
      tick = 0;   
   }
   else
   {
      EICRB |= (1<<ISC41)|(1<<ISC40);// INT4 Rising Edge / INT Set
      pulse_end = pulse_tick;   
   }
}

unsigned char* ButtonInput(void)
{
   unsigned char in;
   unsigned char up;
   unsigned char down;
   unsigned char safe;
   unsigned char* Button_Info;
      
   // 마스크를 이용해 포트D 입력에서 0번핀 입력 추출 
   in = PINE & 0x01;

   if(in>0) up=0;   // S2버튼 안 눌러짐 
   else up=1;      // S2버튼 눌러짐

   // 마스크를 이용해 포트D 입력에서 1번핀 입력 추출 
   in = PINE & 0x02;

   if(in>0) down=0;   // S3버튼 안 눌러짐   
   else down=1;      // S3버튼 눌러짐




   if(up==1 && down==0) Button_Info= "UP";   // S2버튼 누른 경우
   else if(up==0 && down==1) Button_Info = "DN";   // S3버튼 누른 경우
   else if (up == 1 && down == 1) Button_Info = "BO"; // S4버튼 누른 경우
   else Button_Info = "NO";
   return Button_Info;
}

void rt_OneStep(void)
{
  static boolean_T OverrunFlag = 0;

  /* Disable interrupts here */

  /* Check for overrun */
  if (OverrunFlag) {
    rtmSetErrorStatus(CAN_NODE1_AUTO_M, "Overrun");
    return;
  }

  OverrunFlag = TRUE;

  /* Save FPU context here (if necessary) */
  /* Re-enable timer or interrupt here */
  /* Set model inputs here */
  CAN_NODE1_AUTO_U.ADdata = (double)ADdata;
  CAN_NODE1_AUTO_U.button_input_acc = (PINE & 0x01) > 0 ? 0 : 1;
  CAN_NODE1_AUTO_U.button_input_slow = (PINE & 0x02) > 0 ? 0 : 1;
  CAN_NODE1_AUTO_U.distande = distance2;

  /* Step the model */
  CAN_NODE1_AUTO_step();

  /* Get model outputs here */
   /* Outport: '<Root>/current_mode' */
  current_mode = (int)CAN_NODE1_AUTO_Y.current_mode;

  /* Outport: '<Root>/voltage' */
  voltage = (int)CAN_NODE1_AUTO_Y.voltage;

  /* Outport: '<Root>/msg2_0' */
  msg2.data[0] = (int)CAN_NODE1_AUTO_Y.msg2_0;

  /* Outport: '<Root>/msg2_4' */
  msg2.data[4] = (int)CAN_NODE1_AUTO_Y.msg2_4;

  /* Outport: '<Root>/msg1_0' */
  msg1.data[0] = (int)CAN_NODE1_AUTO_Y.msg1_0;

  /* Outport: '<Root>/portb' */
  PORTB = CAN_NODE1_AUTO_Y.portb;

  distance_check = (int)CAN_NODE1_AUTO_Y.distance_check;
  /* Indicate task complete */
  OverrunFlag = FALSE;

  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}

int main(void)
{
   unsigned int speed=3;
   //unsigned int mode = 0; //0은 wait, 1은 speed up, 2는 speed down
   unsigned char thousand, hundred, ten, one;
   float distance;
   unsigned char position = 0;
   unsigned char distenceCheck=0;
   unsigned int dir = 2;
   unsigned int accTime = 0;
   unsigned int tempTime = 0;
   char temp[6];
   
   unsigned int firstMove = 0;

	init_EXT_INT();
   init_TIMER0_COMPA();

   initPort();      // 입출력 포트 초기화
   LCD_init();     // LCD 초기화
   can_init(b250k   );   // CAN 보레이트를 원하는 값으로 세팅한다.
   initAdc();      // AD 변환 초기화


   can_init(5);       // 초기화
   can_init_8Mhz(5);
   can_rx_set( 3, 0x03, EXT, 8, 0x00, 0x05  );    //only 0x03 message recieve from MCU2
   can_rx_set( 4, 0x04, EXT, 8, 0x00, 0x05  );

   sei();         // INT 인에이블 

   //LCD_Write(1,0,"Butten : ");
   LCD_Write(0,1,"Mode : ");


   unsigned char* out;
	char tempVoltage;
   
   

	//디버그용 코드
	msg3.data[0] = 1;
	current_mode=0;

   while(1)
   {

	
	
   
//-------현재 모드출력 부분-------- 

	char a = (PINE & 0x01) > 0 ? 0 : 1;
	char b = (PINE & 0x02) > 0 ? 0 : 1;
     
      if (a == 1) {
	  out = mode[1];
         //current_mode = 1;//SPEED_UP
		 
      }
      else if(b == 1) {
		out = mode[2];
         //current_mode = 2;//SPEED_DOWN
		 
      }
	  else if(a == 1 && b == 1){
	  	 out = mode[0];
		 //current_mode = current_mode;
		 //current_mode = 0;//Wait
	  }
      else {
         //current_mode = current_mode;

      }
	  
	  if(current_speed == 0){//속도가 0이면 대기상태 
		//current_mode = 0;//Wait
	  }
	 
//------모드출력 끝------


	 
	 tempVoltage = (char)voltage;
	 sprintf(temp,"%d",tempVoltage);

	  LCD_Write(9,0,out);
	  LCD_Write(13,0,temp);
	  LCD_Write(9,1,mode[current_mode]);

	  sprintf(temp,"%d",distance_check);
	  LCD_Write(0,1,temp);

	  sprintf(temp,"%d",msg4.data[2]);
	  LCD_Write(15,1,temp);


	rt_OneStep();
	  can_tx(2, &msg2, 0);
      can_tx(1, &msg1, 0);


	rt_OneStep();


  /****************************************
   : 초음파 센서를 사용하기 위한 소스코드 제공
   ****************************************/
      cli();            // clear interrupt
      PE4_Clear;
      PE4_OUT;         // PE4 pin is output
      us_delay(500);

      PE4_Set;         // output set during 5us
      us_delay(5);

      PE4_Clear;         // PE4 clear
      us_delay(100);

      PE4_IN;            // PE4 pin is input
      us_delay(100);

      sei();            // set interrupt


   /* distance = velocity * time */
   distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;
   
	
	

   /* distance digit display */
      thousand = distance/1000;
      distance = distance - (thousand * 1000);

      distance2 = distance;
   
      hundred = distance /100;
      distance = distance - (hundred * 100);

      ten = distance/10; 
      distance = distance - (ten * 10);

      one = distance;

   char part[5];
   part[0] = thousand + '0';
   part[1] = hundred + '0';
   part[2] = ten + '0';
   part[3] = one + '0';
   part[4] = '\0';
   LCD_Write(0,0, part);
	  
	   
      }         
     
   }

