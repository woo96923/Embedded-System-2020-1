/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project5-1
edit : wwChoi
---------------------------------------------------------------------------------------------
[ CAN�۽� ]

Experiment1 -> : ���̽�ƽ�� �̿��Ͽ� X��, Y�ప�� CAN�� �̿��Ͽ� �����Ѵ�. ( LCD�� �ڽ��� ������ �����͸� ǥ���Ѵ�. )

--------------------------------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>         // sprintf�� ����Ǿ� ���� 
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
// AD ��ȯ 
// PF0: ADC0 ���̽�ƽ y�� 
// PF1: ADC1 ���̽�ƽ x��
//=========================================================

void initAdc(void){
   ADMUX =  0x40;  // ��������; �ܺ� ĳ�۽��� ���� AVcc(AREF ��)
               // AD��ȯ ������ ����; ������ ���� 
               // AD��ȯ ä�� ����; PortF 0 ��    

   DDRF  = 0xf0;   // PortF[3..0] �Է����� ����, PortF[7..4] ������� ���� 
   DIDR0 = 0x0f;   // ������ �Է� �Ұ� PortF[3..0]

   ADCSRA= 0xc8;     // ADC �ο��̺�, ADC ��ȯ ����, ADC���ͷ�Ʈ �ο��̺� 
               // ADC Ŭ�� ����; XTAL��1/2(8MHz)
}

void usart1_init(void)
{
	UCSR1A=	0x00; // serial states clear

	UCSR1B=	0x98; // ���ſϷ�INT, ����, �۽� �ο��̺� 

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





SIGNAL(ADC_vect)
{
  ADdata = ADC;
  ADCSRA = 0xc8;
}

void ADcLCD(void)
{
      volume= ADdata; // �������� PortF0
      voltage= volume*50/1023;      
   
}

void initPort(void)
{
   DDRC  = 0xff;   // LCD ������ �� ��� 
   PORTC = 0x00;
   DDRG  = 0xff;   // LCD ���� ��� (RS, RW, E)
   DDRF  = 0x00;   // �Ƴ��α� �Է� 
   DDRD = 0x00;      //switch �Է� ����
   DDRE  = 0x00; 	//�ܺ� switch �Է¼��� 

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
      
   // ����ũ�� �̿��� ��ƮD �Է¿��� 0���� �Է� ���� 
   in = PINE & BUTTON0_MASK;

   if(in>0) up=0;   // S2��ư �� ������ 
   else up=1;      // S2��ư ������

   // ����ũ�� �̿��� ��ƮD �Է¿��� 1���� �Է� ���� 
   in = PINE & BUTTON1_MASK;

   if(in>0) down=0;   // S3��ư �� ������   
   else down=1;      // S3��ư ������




   if(up==1 && down==0) Button_Info= "UP";   // S2��ư ���� ���
   else if(up==0 && down==1) Button_Info = "DN";   // S3��ư ���� ���
   else if (up == 1 && down == 1) Button_Info = "BO"; // S4��ư ���� ���
   else Button_Info = "NO";
   return Button_Info;
}

int main(void)
{
   initPort();      // ����� ��Ʈ �ʱ�ȭ
   LCD_init();     // LCD �ʱ�ȭ
   can_init(b250k   );   // CAN ������Ʈ�� ���ϴ� ������ �����Ѵ�.
   initAdc();      // AD ��ȯ �ʱ�ȭ
   sei();         // INT �ο��̺� 

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
     ADcLCD();//�������� �д��Լ�

     
     
//-------���� ��弳�� �κ�-------- 

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
	  
	  if(current_speed == 0){//�ӵ��� 0�̸� ������ 
		//current_mode = 0;//Wait
	  }
	 
//------��弳�� ��------

	 
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
     msg2.data[4] = tempVoltage; //�������� �� ���� 
     msg2.data[5] = '\0'; 
     msg2.data[6] = '\0';
     msg2.data[7] = '\0';

	  can_tx(2, &msg2, 0);
      can_tx(1, &msg1, 0);

	 //can_rx(3, &msg3);
	  //can_rx(4, &msg4);
	  
      if(current_mode == 0) {//���� ���°� Wait�̸� �������� �ʿ���NODE2���� �޼��� �ޱ�
         can_rx(4, &msg4);//data = {���� ������ ����, \0, ��� �˸�, \0\0\0\0\0\ }
		 //LCD_Write_char(13,0,msg4.data[2]);
		//���������ʿ����� ���� ��忡 ���� ����� ���������
		if(msg4.data[2] == 1){//���� ��� �˸��� 1�̸�
			PORTB = 0x00; //�� Ű�°�
			 
		}
		else if(msg4.data[2] == 0) {
		    PORTB = 0xff;
		} 
		LCD_Write(13,1,msg4.data[2]+'0');
         }
		 
	  //else
	  
	  if(current_mode != 0){//������ �϶�

        can_rx(3, &msg3);//data = {���� ���, \0, ���� �ӵ�, \0\0\0\0\0\ }
		//DC�����ʿ����� ���� ��忡 ���� ����� ���������
		current_speed = msg3.data[2];//�ӵ� ����
		current_mode =  msg3.data[0];//��� ���� 


       }
	   
      }         
     
   }

