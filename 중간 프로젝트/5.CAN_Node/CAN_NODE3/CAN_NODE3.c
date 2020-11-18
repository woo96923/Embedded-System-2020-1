#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>			// sprintf�� ����Ǿ� ���� 

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
         
   UCSR1B=   0b10011000;// ���ſϷ�INT, ����, �۽� �ο��̺� 

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
USART ��� �Լ� ����. BUT ������ ���� �� ������ ���� ÷��
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
   OCR0A = 19;      // 1cycle -> 20us���� ���� 1cycle = 1/(16Mhz/(2*8*( OCR0A+1)))
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
	ADdata= ADC;	// AD��ȯ �����͸� ADdata �� ���� ( Register�� �ִ°��� �������� ADdata�� �Ű� ����ϱ� ���� �̿��Ѵ�. )
   	ADCSRA= 0xC8;   	    // ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺� 
					// ADC Ŭ�� ����; XTAL��1/2(8MHz)
}
void initPort(void)
{
	DDRB  = 0xff; //LED ���� 
	DDRC  = 0xff; //LCD ������ �� ���
	PORTC = 0x00;   //LCD�� ����Ǿ� �����͸� �������Ƿ� ��� ����
	DDRG  = 0xff; //LCD ���� ���
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
	initPort();		// ����� ��Ʈ �ʱ�ȭ 
    LCD_init();     // LCD �ʱ�ȭ
	LCD_cmd(0x01);	// LCD ȭ�� ��
	LCD_Write(0,0, table1);	// �ʱ� ���� ���
	ms_delay(1000);
   	init_EXT_INT();
   	init_TIMER0_COMPA();
	usart1_init();
	can_init(5); 		// �ʱ�ȭ
	can_init_8Mhz(5);
	//sei();

	
	can_rx_set(2, 0x02, EXT, 8, 0x00, 0x05  ); 	// CAN ���ű� �ʱ�ȭ 
		
	//LCD_Write(0,1,talbe2);
	//sei();			// INT �ο��̺� 
	
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
		/*�Ÿ� ���� �� ����*/
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
