/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project4
edit : wwChoi
---------------------------------------------------------------------------------------------

 Experiment1 -> ������ ������ �̿��Ͽ� �� ���� CLCD�� ǥ��
 Experiment2 -> ������ ������ �̿��Ͽ� �� ���� UART������� Tera term�� ǥ��
--------------------------------------------------------------------------------------------*/

#define Experiment 1


#include <avr/io.h>
#include <avr/interrupt.h>
#include "delay.h"
#include "lcd.h"


#define PE4_Clear	(PORTE &= 0xEF)
#define PE4_Set		(PORTE |= 0x10)
#define PE4_IN		(DDRE &= 0xEF)
#define PE4_OUT		(DDRE |= 0x10)




#define TEMPERATURE 25


unsigned short tick=0, pulse_check=0, pulse_end=0;



  //============================================
  // serial com.
void usart1_init(void)
{
	UCSR1A=	0x00; // serial states clear

	UCSR1B=	0x98; // ���ſϷ�INT, ����, �۽� �ο��̺� 

	UCSR1C=	0x06; // stop bit; 0, no parity, 8bits set

	UBRR1H=	0x00; // 16MHz 9600bps set

	UBRR1L=	103;  // 16MHz 9600bps set
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
	EIMSK |= (1<<INT4);		// INT4 Interrupt Set
	EICRB |= (1<<ISC41)|(1<<ISC40);		//INT4 Rising Edge /INT Set
	EIFR   = 1<<INT4;		// Interrupt Flag
}

void init_TIMER0_COMPA(void)
{
	TCCR0A |= (1<<WGM01);		// CTC Mode
	TCCR0A |= (1<<CS01);	// clk/8 Prescaler
	TCNT0 = 0;
	OCR0A = 19;		// 1cycle -> 20us���� ���� 1cycle = 1/(16Mhz/(2*8*( OCR0A+1)))
	TIMSK0 = (1<<OCIE0A);		//T/C0 Output Compare Match INT
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
	   EICRB &= 0x00;	//  low state
	   tick = 0;	
	}
	else
	{
		EICRB |= (1<<ISC41)|(1<<ISC40);// INT4 Rising Edge / INT Set
		pulse_end = pulse_tick;	
	}
}

//======================================
// port init
void initPort(void)
{
	DDRC  = 0xff; // LCD ������ ��� 
	PORTC = 0x00;

	DDRG  = 0x0f; // LCD ���� ��� 

	DDRE  = 0xf0; //�̵��� Ȯ���ϼ�
	PORTE = 0xff; //�̰͵� 
}

int main(void)
{
	unsigned char thousand, hundred, ten, one;
	float distance;
	
	initPort();
	init_EXT_INT();
	init_TIMER0_COMPA();
	usart1_init();

#if Experiment == 1
	LCD_init();
	LCD_cmd(0x01);
	ms_delay(50);
	LCD_Write(0, 0, "AJOUUNIV Serial");
	LCD_Write(0, 1, "Distance:");
	
	ms_delay(200);
#endif

	while(1)
	{
	/****************************************
	: ������ ������ ����ϱ� ���� �ҽ��ڵ� ����
	****************************************/
		cli();				// clear interrupt
		PE4_Clear;
		PE4_OUT;			// PE4 pin is output
		us_delay(500);

		PE4_Set;			// output set during 5us
		us_delay(5);

		PE4_Clear;			// PE4 clear
		us_delay(100);

		PE4_IN;				// PE4 pin is input
		us_delay(100);

		sei();				// set interrupt


	/* distance = velocity * time */
	distance = (331.5+(0.6*TEMPERATURE))*(pulse_end*0.00001/2)*1000;

	/* distance digit display */
		thousand = distance/1000;
		distance = distance - (thousand * 1000);
	
		hundred = distance /100;
		distance = distance - (hundred * 100);

		ten = distance/10; 
		distance = distance - (ten * 10);

		one = distance;

#if Experiment == 1
		/*************************************
		����1 �ҽ��ڵ�
		LCD Display �Լ��� �̿��Ͽ� ������ ������ ǥ��
		*************************************/

	   	char prt[5];
     	prt[0]=thousand + '0';
      	prt[1]=hundred + '0';
      	prt[2]=ten + '0';
      	prt[3]=one + '0';
      	prt[4] = '\0';
      	LCD_Write(10, 1, prt);
#endif

#if Experiment == 2
		/**************************************************
		����2 �ҽ��ڵ�
		usart1_transmit()�Լ��� �̿��Ͽ� ������ ���� ���� 
		ASCII ����
		***************************************************/

		usart1_transmit_string("Distance : ");
      usart1_transmit(thousand+ '0');
      usart1_transmit(hundred+ '0');
      usart1_transmit(ten+ '0');
      usart1_transmit(one+ '0');
      usart1_transmit('\n');
     usart1_transmit('\r');
		

#endif


		ms_delay(100);	
	}
	return 0;
}
