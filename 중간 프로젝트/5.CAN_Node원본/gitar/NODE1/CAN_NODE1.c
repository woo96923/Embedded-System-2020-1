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
unsigned int voltage_x, voltage_y;
unsigned int volume_x, volume_y;
unsigned char ADCFlag;

struct MOb msg1={0x01, 0, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};    
struct MOb msg2={0x01, 0, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
struct MOb msg3={0x01, 0, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
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
char mode[4][9] = { "PRK", "RVS", "NTR", "DRV"};
int current_mode;
int current_speed;


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

SIGNAL(ADC_vect)
{
  ADdata = ADC;
  ADCSRA = 0xc8;
}

void ADcLCD(void)
{
   if(ADCFlag == 0)
   { 
      volume_y= ADdata; // �������� PortF0
      voltage_y= volume_y*50/1023;
      ADCFlag=1; 
      ADMUX = 0x40;   // PortF1 �Ƴ��α� �Է� ���� 
   } 
   else if(ADCFlag == 1)
   { 
      volume_x= ADdata; // �µ����� PortF1
      voltage_x= volume_x*50/1023;
      ADCFlag=0; 
      ADMUX = 0x41;   // PortF2 �Ƴ��α� �Է� ���� 
   }
   else ADCFlag=0;
}

void initPort(void)
{
   DDRC  = 0xff;   // LCD ������ �� ��� 
   PORTC = 0x00;
   DDRG  = 0xff;   // LCD ���� ��� (RS, RW, E)
   DDRF  = 0x00;   // �Ƴ��α� �Է� 
   DDRD = 0x00;		//switch �Է� ����
   DDRE  = 0x00;
}


/******************
���� �ʿ�
*******************/
unsigned char* ButtonInput(void)
{
	unsigned char in;
	unsigned char up;
	unsigned char down;
	unsigned char* Button_Info;
		
	// ����ũ�� �̿��� ��ƮD �Է¿��� 0���� �Է� ���� 
	in = PIND & BUTTON0_MASK;

	if(in>0) up=0;	// S2��ư �� ������ 
	else up=1;		// S2��ư ������

	// ����ũ�� �̿��� ��ƮD �Է¿��� 1���� �Է� ���� 
	in = PIND & BUTTON1_MASK;

	if(in>0) down=0;	// S3��ư �� ������	
	else down=1;		// S3��ư ������

	if(up==1 && down==0) Button_Info= "LEFT";	// S2��ư ���� ���
	else if(up==0 && down==1) Button_Info = "RIGHT";	// S3��ư ���� ���
	else if (up == 1 && down == 1) Button_Info = "BOTH";
	else Button_Info = "NTH";
	return Button_Info;
}

int main(void)
{
   initPort();      // ����� ��Ʈ �ʱ�ȭ
    LCD_init();     // LCD �ʱ�ȭ
   can_init(b250k   );   // CAN ������Ʈ�� ���ϴ� ������ �����Ѵ�.
   initAdc();      // AD ��ȯ �ʱ�ȭ
   sei();         // INT �ο��̺� 
   current_mode = 0;
   current_speed = 1;
   int menu_flag = 0;
   	int speed_flag = 0;
	int change_y = 0;
	int change_x = 0;
	int twink = 0;
	unsigned int out=0;
	int t = 0;
	char num[8] = "00000000";
	int back_distance;
	char dist[4] = "0000";
    LCD_Write(0,0, "before");
	ms_delay(500);	
	can_rx_set( 1, 0x01, EXT, 8, 0x00, 0  ); 
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
      ADcLCD();
	
	  change_y = 0;
	  change_x = 0;
		can_rx(1, &msg2);
      //Print Current Mode & Speed to LCD
	  if( (current_mode == 0 ) || (current_mode == 2) ){
	    LCD_Write(0, 0, mode[current_mode] );
		LCD_Write(0, 1, "           ");	 
		current_speed = 0;
	  }
	  else{
	  	LCD_Write(0, 0, mode[current_mode] );
		
	  }

	  
	  if( voltage_y > 40 ) menu_flag = 1;
	  else {
		if(menu_flag == 1) {
			change_y = 1;
			change_y = 1;
			menu_flag = 0;
			current_mode = current_mode + 1;
			if( current_mode >= 3) current_mode = 3;
		}
	  }
	
	  if ( voltage_y < 10 ) menu_flag = -1;
	  else {
		if(menu_flag == -1) {
			change_y = 1;
			menu_flag = 0;
			current_mode = current_mode - 1;
			if( current_mode <= 1) current_mode = 1;
		}

	  }

	  //when driving mode, speed control
	  if( (current_mode == 3) || (current_mode == 1) ){

		num[7] = '0' + current_speed%10;
		num[6] = '0' + current_speed%100/10;
		num[5] = '0' + current_speed%1000/100;
		num[4] = '0' + current_speed%10000/1000;
		num[3] = '0' + current_speed%100000/10000;
		num[2] = '0' + current_speed%1000000/100000;
		num[1] = '0' + current_speed%10000000/1000000;
		num[0] = '0' + current_speed%100000000/10000000;
		LCD_Write(0, 1, num);

		if(voltage_x > 40) speed_flag = 1;
		else {
			if(speed_flag == 1) {
				change_x = 1;
				speed_flag = 0;
				current_speed = current_speed + 1;
				if ( current_speed >= 5) current_speed = 5;
			}
		}

		if(voltage_x < 10) speed_flag = -1;
		else {
			if(speed_flag == -1) {
				change_x = 1;
				speed_flag = 0;
				current_speed = current_speed - 1;
				if ( current_speed <= 0) current_speed = 0;
			}
		}
	  }

	  out = ButtonInput();
		if (out == "LEFT") {
			twink = 1;	
			t = 1;
		}
		else if(out == "RIGHT") {
			twink = 1;
			t = 2;
		}
		else if(out == "BOTH") {
			twink = 1;
			t = 3;
		}
		else {
			twink = 0;
			t = 0;
		}
		///added
		if(current_mode == 1) {
			
			back_distance = msg2.data[0];
			//if(back_distance <= 30) {
			//	current_speed = 1;
			//}
			dist[3] = back_distance%10 + '0';
			dist[2] = '0' + back_distance%100/10;
			dist[1] = '0' + back_distance%1000/100;
			dist[0] = '0' + back_distance%10000/1000;
			LCD_Write(8, 1, dist);
		}			
	  
	  msg1.data[0] = current_mode;
      msg1.data[1] = '\0'; 
	  msg1.data[2] = current_speed; 
	  msg1.data[3] = '\0'; 
	  msg1.data[4] = current_mode%10 + '0'; 
	  msg1.data[5] = '\0'; 
	  msg1.data[6] = t%10 + '0';
	  //added
	  msg1.data[7] = '\0'; 

	  //msg2.data[6] = t%10 + '0';	

	  if(change_y || change_x || twink) {
      	can_tx(0, &msg1, 0);
		//added
		//can_tx(2, &msg2, 0);
	  }
   }
}
