#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>			// sprintf문 선언되어 있음 

#include "myDelay.h"
#include "lcdControl.h"
#include "myCANLib.h"

//#include "lcdControl.c"
//#include "myCANLib.c"

#define PE4_Clear   (PORTE &= 0xEF)
#define PE4_Set      (PORTE |= 0x10)
#define PE4_IN      (DDRE &= 0xEF)
#define PE4_OUT      (DDRE |= 0x10)
#define TEMPERATURE 25
unsigned int ADdata;


unsigned short tick=0, pulse_check=0, pulse_end=0;

char strBuff[20]={0};

struct MOb msg2;    
struct MOb msg3={3,0x03, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};   

void usart1_init(void)
{
   UCSR1A=   0b00000000;// serial states clear
         
   UCSR1B=   0b10011000;// 수신완료INT, 수신, 송신 인에이블 

   UCSR1C=   0b00000110;// stop bit; 0, no parity, 8bits set

   UBRR1H=   0b00000000;// 16MHz 9600bps set

   UBRR1L=   0b01100111;// 16MHz 9600bps set
}

  /* Read and write functions */
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

//========================================
// ultra sound 
void init_EXT_INT(void)
{
   EIMSK |= (1<<INT4);      // INT4 Interrupt Set
   EICRB |= (1<<ISC41)|(1<<ISC40);      //INT4 Rising Edge /INT Set
   EIFR   = 1<<INT4;   // Interrupt Flag
}
void init_TIMER0_COMPA(void)
{
   TCCR0A |= (1<<WGM01);      // CTC Mode
   TCCR0A |= (1<<CS01);      // clk/8 Prescaler
   TCNT0 = 0;
   OCR0A = 19;      // 1cycle -> 20us으로 세팅 1cycle = 1/(16Mhz/(2*8*( OCR0A+1)))
   TIMSK0 = (1<<OCIE0A);      //T/C0 Output Compare Match INT
   TIFR0 = 0b00000000;
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

SIGNAL(ADC_vect)
{
	ADdata= ADC;	// AD변환 데이터를 ADdata 에 저장 ( Register에 있는값을 전역변수 ADdata에 옮겨 사용하기 쉽게 이용한다. )
   	ADCSRA= 0xC8;   	    // ADC 인에이블, ADC 변환 시작, ADC인터럽트 인에이블 
					// ADC 클럭 설정; XTAL의1/2(8MHz)
}
void initPort(void)
{
	DDRB  = 0xff; //LED 제어 
	DDRC  = 0xff; //LCD 데이터 및 명령
	PORTC = 0x00;   //LCD에 연결되어 데이터를 내보내므로 출력 설정
	DDRG  = 0xff; //LCD 제어 출력
	DDRF  = 0x00; 
	DDRE  = 0xff;
    PORTE = 0xff;
}




int main(void)
{	
	PORTB = 0xff;
	char table1[10] = "Distance :";
	unsigned char thousand, hundred, ten, one;
   	float distance;
	float temp_distance = 0;
	unsigned int twink = 0;
   	unsigned char num[5];
	int count = 0;
	initPort();		// 입출력 포트 초기화 
    LCD_init();     // LCD 초기화
	LCD_cmd(0x01);	// LCD 화면 지
	LCD_Write(0,0, table1);	// 초기 글자 출력
	ms_delay(1000);
   	init_EXT_INT();
   	init_TIMER0_COMPA();
	usart1_init();
	can_init(5); 		// 초기화
	can_init_8Mhz(5);
	//sei();

	
	can_rx_set(2, 0x02, EXT, 8, 0x00, 0x05  ); 	// CAN 수신기 초기화 
		
	//LCD_Write(0,1,talbe2);
	//sei();			// INT 인에이블 
	
	while(1)
	{
		
		//added
	  can_rx(2, &msg2);
		

	if(msg2.data[0] == 1) {
	temp_distance = 0;
	while(count <= 50)
	{
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
		distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;
   		temp_distance = temp_distance + distance;
	  count = count+1;
	}
   temp_distance = temp_distance/10;
   distance = temp_distance;
   count = 0;
   /* distance digit display */
      thousand = distance/1000;
      distance = distance - (thousand * 1000);
   
      hundred = distance /100;
      distance = distance - (hundred * 100);

      ten = distance/10; 
      distance = distance - (ten * 10);
      one = distance;
	  num[0] = thousand + '0';
      num[1] = hundred + '0';
      num[2] = ten + '0';
      num[3] = one + '0';
      num[4] = '\0';
      LCD_Write(0, 1, num);
		/*거리 측정 후 전송*/
		msg3.data[0] = temp_distance;
		msg3.data[1] = '\0';
		msg3.data[2] = '\0';
		msg3.data[3] = '\0';
		msg3.data[4] = '\0';
		msg3.data[5] = '\0';
		msg3.data[6] = '\0';
		msg3.data[7] = '\0';
		can_tx(3,&msg3,0);
		/************from MCU3 to MCU1***************/
		
	} 
		/*if node1 send message 'r',then turn on LED*/
		//added
		if( ( msg2.data[6] == '2' ) || ( msg2.data[6] == '3')) {
         twink = 1;
      	}
      	else twink = 0;

      	if(twink == 1) {
         	PORTB = 0x00;

      }
	  else PORTB = 0xff;
	}
}
