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
unsigned int voltage_x, voltage_y;
unsigned int volume_x, volume_y;
unsigned char ADCFlag;

struct MOb msg1={1, 0x01, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};    //to MCU2
struct MOb msg2={2, 0x02, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};   //to MCU3
struct MOb msg3;   //from MCU3
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

SIGNAL(ADC_vect)
{
  ADdata = ADC;
  ADCSRA = 0xc8;
}

void ADcLCD(void)
{
   if(ADCFlag == 0)
   { 
      volume_y= ADdata; // 가변저항 PortF0
      voltage_y= volume_y*50/1023;
      ADCFlag=1; 
      ADMUX = 0x40;   // PortF1 아날로그 입력 선택 
   } 
   else if(ADCFlag == 1)
   { 
      volume_x= ADdata; // 온도센서 PortF1
      voltage_x= volume_x*50/1023;
      ADCFlag=0; 
      ADMUX = 0x41;   // PortF2 아날로그 입력 선택 
   }
   else ADCFlag=0;
}

void initPort(void)
{
   DDRC  = 0xff;   // LCD 데이터 및 명령 
   PORTC = 0x00;
   DDRG  = 0xff;   // LCD 제어 출력 (RS, RW, E)
   DDRF  = 0x00;   // 아날로그 입력 
   DDRD = 0x00;      //switch 입력 설정
   DDRE  = 0x00;
}

unsigned char* ButtonInput(void)
{
   unsigned char in;
   unsigned char up;
   unsigned char down;
   unsigned char* Button_Info;
      
   // 마스크를 이용해 포트D 입력에서 0번핀 입력 추출 
   in = PIND & BUTTON0_MASK;

   if(in>0) up=0;   // S2버튼 안 눌러짐 
   else up=1;      // S2버튼 눌러짐

   // 마스크를 이용해 포트D 입력에서 1번핀 입력 추출 
   in = PIND & BUTTON1_MASK;

   if(in>0) down=0;   // S3버튼 안 눌러짐   
   else down=1;      // S3버튼 눌러짐

   if(up==1 && down==0) Button_Info= "LEFT";   // S2버튼 누른 경우
   else if(up==0 && down==1) Button_Info = "RIGHT";   // S3버튼 누른 경우
   else if (up == 1 && down == 1) Button_Info = "BOTH";
   else Button_Info = "NTH";
   return Button_Info;
}

int main(void)
{
   initPort();      // 입출력 포트 초기화
    LCD_init();     // LCD 초기화
   can_init(b250k   );   // CAN 보레이트를 원하는 값으로 세팅한다.
   initAdc();      // AD 변환 초기화
   sei();         // INT 인에이블 
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
   can_rx_set( 3, 0x03, EXT, 8, 0x00, 0x05  );    //only 0x03 message recieve from MCU3
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
     if( (current_mode == 0 ) || (current_mode == 2) ){
       LCD_Write(0, 0, mode[current_mode] );
      LCD_Write(0, 1, "           ");    
      current_speed = 0;
     }
     else{
	 	current_speed=1;
        LCD_Write(0, 0, mode[current_mode] );
      
     }

     
     if( voltage_y > 40 ) menu_flag = 1;
     else {
      if(menu_flag == 1) {
         menu_flag = 0;
         current_mode = current_mode + 1;
         if( current_mode >= 3) current_mode = 3;
      }
     }
     
     if ( voltage_y < 10 ) menu_flag = -1;
     else {
      if(menu_flag == -1) {
         menu_flag = 0;
         current_mode = current_mode - 1;
         if( current_mode <= 0) current_mode = 0;
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
            speed_flag = 0;
            current_speed = current_speed + 1;
            if ( current_speed >= 5) current_speed = 5;
         }
      }

      if(voltage_x < 10) speed_flag = -1;
      else {
         if(speed_flag == -1) {
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
	  //From MCU1 to MCU2
	 msg1.data[0] = current_mode;
     msg1.data[1] = '\0'; 
     msg1.data[2] = current_speed; 
     msg1.data[3] = '\0'; 
     msg1.data[4] = current_mode%10 + '0'; 
     msg1.data[5] = '\0'; 
     msg1.data[6] = t%10 + '0';
     msg1.data[7] = '\0'; 
	 //From MCU1 to MCU3
     msg2.data[0] = current_mode;
     msg2.data[1] = '\0'; 
     msg2.data[2] = current_speed;  
     msg2.data[3] = '\0'; 
     msg2.data[4] = current_mode%10 + '0'; 
     msg2.data[5] = '\0'; 
     msg2.data[6] = t%10 + '0';
     msg2.data[7] = '\0';
	  can_tx(2, &msg2, 0);
      can_tx(1, &msg1, 0);
      if(current_mode == 1) {
         can_rx(3, &msg3);

         back_distance = msg3.data[0];
         if(back_distance <= 5 && back_distance > 0) {
            current_speed = 1;

         }
      }         
     
   }
}
