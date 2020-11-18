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

#define BUTTON0_MASK 0x01
#define BUTTON1_MASK 0x02

/*
#include "myDelay.c"

#include "myCANLib.c"
*/
unsigned int ADdata; 
unsigned int voltage; //_x, voltage_y;
unsigned int volume;//_x, volume_y;
unsigned char ADCFlag;

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
char mode[3][12] = { "WAIT", "ACCR", "SLOW"};
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

unsigned char* ButtonInput(void)
{
   unsigned char in;
   unsigned char up;
   unsigned char down;
   unsigned char safe;
   unsigned char* Button_Info;
      
   // 마스크를 이용해 포트D 입력에서 0번핀 입력 추출 
   in = PINE & BUTTON0_MASK;

   if(in>0) up=0;   // S2버튼 안 눌러짐 
   else up=1;      // S2버튼 눌러짐

   // 마스크를 이용해 포트D 입력에서 1번핀 입력 추출 
   in = PINE & BUTTON1_MASK;

   if(in>0) down=0;   // S3버튼 안 눌러짐   
   else down=1;      // S3버튼 눌러짐




   if(up==1 && down==0) Button_Info= "UP";   // S2버튼 누른 경우
   else if(up==0 && down==1) Button_Info = "DN";   // S3버튼 누른 경우
   else if (up == 1 && down == 1) Button_Info = "BO"; // S4버튼 누른 경우
   else Button_Info = "NO";
   return Button_Info;
}

int main(void)
{
   initPort();      // 입출력 포트 초기화
   LCD_init();     // LCD 초기화
   can_init(b250k   );   // CAN 보레이트를 원하는 값으로 세팅한다.
   initAdc();      // AD 변환 초기화
   sei();         // INT 인에이블 

   LCD_Write(1,0,"Butten : ");
   LCD_Write(0,1,"Mode : ");

	current_mode=0;
	current_speed=0;

   unsigned char* out;
   char temp[5];
	char tempVoltage;
   
   can_rx_set( 3, 0x03, EXT, 8, 0x00, 0x05  );    //only 0x03 message recieve from MCU2
   can_rx_set( 4, 0x04, EXT, 8, 0x00, 0x05  );
   while(1)
   {
      /********************************************
      /*NODE1
      /* print menu
     /* controlled by joystick
     /* x : voltage_x , y : voltage_y
     /* current_mode : 0. parking 1. reverse 2. neutral 3. driving
                   default : 0. parking
     /* 
      ********************************************/
     ADcLCD();//가변저항 읽는함수

     
     
//-------현재 모드설정 부분-------- 

     out = ButtonInput();
      if (out == "UP") {
         current_mode = 1;//SPEED_UP
		 
      }
      else if(out == "DN") {
         current_mode = 2;//SPEED_DOWN
		 
      }
	  else if(out == "BO"){
	  	 //current_mode = current_mode;
		 current_mode = 0;//Wait
	  }
      else {
         current_mode = current_mode;

      }
	  
	  if(current_speed == 0){//속도가 0이면 대기상태 
		//current_mode = 0;//Wait
	  }
	 
//------모드설정 끝------

	 
	 tempVoltage = (char)voltage;
	 sprintf(temp,"%d",tempVoltage);

	  LCD_Write(9,0,out);
	  LCD_Write(13,0,temp);
	  LCD_Write(9,1,mode[current_mode]);





	  //From MCU1 to MCU2
	 msg1.data[0] = current_mode;
     msg1.data[1] = '\0'; 
     msg1.data[2] = current_speed; 
     msg1.data[3] = '\0'; 
     msg1.data[4] = '\0'; 
     msg1.data[5] = '\0'; 
     msg1.data[6] = '\0';
     msg1.data[7] = '\0'; 
	
	 //From MCU1 to MCU3
	 msg2.data[0] = current_mode;
     msg2.data[1] = '\0'; 
     msg2.data[2] = current_speed;  
     msg2.data[3] = '\0'; 
     msg2.data[4] = tempVoltage; //가변저항 값 전달 
     msg2.data[5] = '\0'; 
     msg2.data[6] = '\0';
     msg2.data[7] = '\0';

	  can_tx(2, &msg2, 0);
      can_tx(1, &msg1, 0);

	 //can_rx(3, &msg3);
	  //can_rx(4, &msg4);
	  
      if(current_mode == 0) {//현재 상태가 Wait이면 서보모터 쪽에서NODE2에서 메세지 받기
         can_rx(4, &msg4);//data = {현재 안전바 각도, \0, 경고 알림, \0\0\0\0\0\ }
		 //LCD_Write_char(13,0,msg4.data[2]);
		//서보모터쪽에서도 현재 모드에 따라서 통신을 보내줘야함
		if(msg4.data[2] == 1){//만약 경고 알림이 1이면
			PORTB = 0x00; //불 키는거
			 
		}
		else if(msg4.data[2] == 0) {
		    PORTB = 0xff;
		} 
		LCD_Write(13,1,msg4.data[2]+'0');
         }
		 
	  //else
	  
	  if(current_mode != 0){//동작중 일때

        can_rx(3, &msg3);//data = {현재 모드, \0, 현재 속도, \0\0\0\0\0\ }
		//DC모터쪽에서도 현재 모드에 따라서 통신을 보내줘야함
		current_speed = msg3.data[2];//속도 갱신
		current_mode =  msg3.data[0];//모드 갱신 


       }
	   
      }         
     
   }

