/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project3
edit : wwChoi
---------------------------------------------------------------------------------------------

 Experiment1 ->  SW2 누를시에 Motor 작동, SW3누를시 Motor 정지
 Experiment2 ->  SW2 누를시에 Motor Speed UP, SW3누를시에 Speed Down[ Motor Speed down 최대속도 5까지 ]
--------------------------------------------------------------------------------------------*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>         

#include "lcdControl.h"
#include "myCANLib.h"
#include "myCANLib.c"


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

unsigned char table1[16]="AJOU-UNIVERSITY";
unsigned char table2[16]="SPEED_CONTROL";

char *mode[3][12] = { "WAIT", "ACCR", "SLOW"};

unsigned char strBuff[20]={0};

unsigned char distenceCheck=1;

struct MOb msg1;/////////////
struct MOb msg3 = {3, 0x03, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};////////////


void initPort(void)
{
   DDRC  = 0xff;   // LCD 데이터 및 명령 
   DDRG  = 0x0f;   // LCD 제어 출력 (RS, RW, E)
   DDRD  = 0x00;   // SW 입력설정
   DDRE  = 0xf0;   // Motor를 이용하기 위한 포트. 필요한 핀 출력 설정
   PORTE = 0xff;
}

// DC모터 초기화
void initMotor(void)
{
   TCCR3A=   0b10001010;   // COM3A[1:0]=10,  비교일치시 클리어
            //               TOP 셋 
            // WGM3[3:0] :     Phase correct PWM mode 
            //                 TOP값은 ICR 레지스터가 결정
   TCCR3B=   0b00010011;   // 64 분주 
                
                
   ICR3 = FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD;   
               // 최고값(Top)3125, 40Hz(25msec) 

   OCR3C = 70;
}

// DC모터 속도 제어와 모터 회전 방향 제어 
void CtrlDcMotor(unsigned int speed, unsigned int dir)
{
   unsigned int level=5;

   PORTE&=0xFC;

   // DC모터 회전 방향 결정
   if(dir==0)      // 시계 방향 회전
   {
      // speed에 따른 속도 제어
      OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD)/level;
      PORTE|=0x01;   
   }
   else if(dir==1)   // 반시계 방향 회전 
   {
      // speed에 따른 속도 제어
      OCR3A=(speed*FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD)/level;
      PORTE|=0x02;
   }
   else         // 모터 정지 
   {
      OCR3A=100;
      OCR3B=100;
   }
}
// 포터D의 0번(S3)과 1번(S2) 핀에서 입력 받음 
unsigned char ButtonInput(void)
{
   unsigned char in;
   unsigned char up;
   unsigned char down;
      
   // 마스크를 이용해 포트D 입력에서 0번핀 입력 추출 
   in = PIND & BUTTON0_MASK;

   if(in>0) up=0;   // S2버튼 안 눌러짐 
   else up=1;      // S2버튼 눌러짐

   // 마스크를 이용해 포트D 입력에서 1번핀 입력 추출 
   in = PIND & BUTTON1_MASK;

   if(in>0) down=0;   // S3버튼 안 눌러짐   
   else down=1;      // S3버튼 눌러짐

   if((up==1) && (down==0)) return SPEED_UP;   // S2버튼 누른 경우
   if((up==0) && (down==1)) return SPEED_DOWN;   // S3버튼 누른 경우

   return SPEED_NO_CHANGE;
}


// 모터 속도값 제어 함수 (버튼 입력에 따른)
unsigned char DcMotorSpeedControl(unsigned char bt_in, unsigned char speed)
{
   // S2 버튼이 눌러지면 
   if(bt_in==SPEED_UP) 
   {
      if(speed==SPEED_MAX) return speed;
      return (++speed);   // 속도 1 증가 
   }
   // S3 버튼이 눌러지면
   if(bt_in==SPEED_DOWN)
   {
      if(speed==SPEED_MIN) return speed;
      return (--speed);   // 속도 1 감소 
   }
   return speed;
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

void delay(unsigned int k)
{
    unsigned int i;

   for(i=0;i<k;i++); 
}

void us_delay(unsigned int us_time)
{
   unsigned int i;

   for(i=0; i<us_time; i++) // 4 cycle +
   {
       asm("PUSH R0");    // 2 cycle +
      asm("POP R0");       // 2 cycle +
      asm("PUSH R0");    // 2 cycle +
      asm("POP R0");       // 2 cycle + =12 cycle for 11.0592MHZ
      asm("PUSH R0");    // 2 cycle +
      asm("POP R0");       // 2 cycle = 16 cycle = 1us for 16MHz
   }
}

void ms_delay(unsigned int ms_time)
{
    unsigned int i;
    
    for(i=0; i<ms_time;i++)
        us_delay(1000);
}

int main(void)
{
   unsigned int speed=3;
   unsigned int mode = 0; //0은 wait, 1은 speed up, 2는 speed down
   unsigned char thousand, hundred, ten, one;
   float distance, distance2;
   unsigned char position = 0;
   unsigned char distenceCheck=0;
   unsigned int dir = 2;
   unsigned int accTime = 0;
   unsigned int tempTime = 0;
   char temp[6];

   unsigned int firstMove = 0;


   unsigned char *button;
   can_init(5);       // 초기화
   can_init_8Mhz(5);
   can_rx_set(1, 0x01, EXT, 8, 0x00, 0x05  );       // CAN 수신기 초기화 



   initPort();      // 입출력 포트 초기화
   init_EXT_INT();
   init_TIMER0_COMPA();

    LCD_init();     // LCD 초기화

   initMotor();   // 스텝모터 제어를 위한 타이머/카운터 초기화 

   sei();         // 사용이유에 대해서 보고서에 첨부하기바람.

   LCD_cmd(0x01);   // LCD 화면 지우기 
 
  
      
   while(1)
   {
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
   LCD_Write(10,0, part);

   can_rx(1, &msg1);/////////////////////////////////

   mode = (int)msg1.data[0];///
   LCD_Write(6,0, &msg1.data[0]);

   button = ButtonInput(); 
   if(button == SPEED_UP)
      {mode = 1; LCD_Write(5,0, "UP");
	   firstMove = 1;}
   else if(button == SPEED_DOWN)
      {mode = 2; LCD_Write(5,0, "DN");}

   if (mode == 0) LCD_Write(0,0, "WAIT");//모드 출력
   if (mode == 1) LCD_Write(0,0, "SPUP");//모드 출력
   if (mode == 2) LCD_Write(0,0, "SPDN");//모드 출력

   if (position == 0) LCD_Write(10,1, "R");
   if (position == 1) LCD_Write(10,1, "L");

   


    sprintf(temp,"%d",accTime);
    LCD_Write(0,1, temp);
    sprintf(temp,"%d",tempTime);
    LCD_Write(4,1, temp);
    
  
	  speed = 3;
      if (firstMove == 1)
	  {
	    firstMove = 0;
		tempTime = 0;
	  	CtrlDcMotor(speed, dir);
		delay(300);
	  }
	  else{
	  
		  if(distance2 < 50)
	      {
	         distenceCheck = position;
	         tempTime += 1;

	         if(tempTime > 200) {mode = 0; dir =2; tempTime = 0;}
	      }
	      else if((distance2 > 100)&& (distance2 < 200) && (position == distenceCheck))
	      {
	         accTime = tempTime;
	         tempTime = 0;
	         if (position == 0)
	         {
	            position = 1;
	            if(mode == 1)//speedup
	            dir = 1;
	            else if(mode == 2)//speed down
	            dir = 0;
	         }
	         else if (position == 1)
	         {
	            position = 0;
	            if(mode == 1)//speedup
	            dir = 0;
	            else if(mode == 2)//speed down
	            dir = 1;
	         }
	      }
   
	   }
      
      


      CtrlDcMotor(speed, dir);

          //From MCU2 to MCU1
	 msg3.data[0] = mode;
     msg3.data[1] = '\0'; 
     msg3.data[2] = tempTime; 
     msg3.data[3] = '\0'; 
     msg3.data[4] = '\0'; 
     msg3.data[5] = '\0'; 
     msg3.data[6] = '\0';
     msg3.data[7] = '\0'; 

	if(mode != 0)
	  can_tx(3, &msg3, 0);


   }


}
