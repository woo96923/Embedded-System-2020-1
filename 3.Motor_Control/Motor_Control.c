/*-------------------------------------------------------------------------------------------
            2015-2 Embedded system experiment

Project name : Project3
edit : wwChoi
---------------------------------------------------------------------------------------------

 Experiment1 ->  SW2 누를시에 Motor 작동, SW3누를시 Motor 정지
 Experiment2 ->  SW2 누를시에 Motor Speed UP, SW3누를시에 Speed Down[ Motor Speed down 최대속도 5까지 ]
--------------------------------------------------------------------------------------------*/
#define Experiment 2

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>         

#include "lcdControl.h"



#define FREQ_CLKIO      16000000   // clock IO 16 MHz
#define PRESCALE      64
#define DC_PERIOD      25         // DC 모터 주기 25 msec

#define BUTTON0_MASK 0x01
#define BUTTON1_MASK 0x02

#define SPEED_NO_CAHNGE   0
#define SPEED_UP       1
#define SPEED_DOWN       2
#define SPEED_MAX      5
#define SPEED_MIN      0


unsigned char table1[16]="AJOU-UNIVERSITY";
#if Experiment == 1
unsigned char table2[16]="ON_OFF_CONTROL";
#elif Experiment ==2
unsigned char table2[16]="SPEED_CONTROL";
#endif

unsigned char strBuff[20]={0};


void initPort(void)
{
   DDRC  = 0xff;   // LCD 데이터 및 명령 
   DDRG  = 0x0f;   // LCD 제어 출력 (RS, RW, E)
   DDRD  = 0x00;   // SW 입력설정
   DDRE  = 0xff;   // Motor를 이용하기 위한 포트. 필요한 핀 출력 설정
}

// DC모터 초기화
void initMotor(void)
{
   TCCR3A=   0b10000010;   // COM3A[1:0]=10,  비교일치시 클리어
            //               TOP 셋 
            // WGM3[3:0] :     Phase correct PWM mode 
            //                 TOP값은 ICR 레지스터가 결정
   TCCR3B=   0b00010011;   // 64 분주 
                
                
   ICR3 = FREQ_CLKIO/2/PRESCALE/1000*DC_PERIOD;   
               // 최고값(Top)3125, 40Hz(25msec) 
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


#if Experiment == 1
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

   if(up==1 && down==0) Button_Info= " ON";   // S2버튼 누른 경우
   else if(up==0 && down==1) Button_Info = "OFF";   // S3버튼 누른 경우
   
   return Button_Info;
}

#elif Experiment == 2
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

   if(up==1 && down==0) return SPEED_UP;   // S2버튼 누른 경우
   if(up==0 && down==1) return SPEED_DOWN;   // S3버튼 누른 경우

   return SPEED_NO_CAHNGE;
}
#endif

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
#if Experiment == 1
   unsigned char* out;
   unsigned int speed=0;

   initPort();   
   LCD_init();
   initMotor();
   sei();                // 보고서 첨부시 이 함수에 대해 설명하시오.

   LCD_cmd(0x01);         // LCD 화면 지우기 
   ms_delay(50);         // 50ms 지연 

   LCD_Write(0,0, table1);   // 초기 글자 출력
   LCD_Write(0,1, table2);

   ms_delay(1500);
   //LCD_cmd(0x01);         // LCD 화면 지우기 
   ms_delay(100);

	speed = 5;
   while(1)
   {   
      /*****************************************
       실험1 코드작성
      : CPU보드의 버튼 S2(+)와 S3(-)를 입력 받음    
      : ButtonInput()함수를 이용하세요.
      : CtrlDcMotor()함수를 이용하세요.
      
      ******************************************/
  /*    
	  out = ButtonInput();
      if (out == " ON")
         speed = 0;
      else if (out == "OFF")
         speed = 5;
   
   	  CtrlDcMotor(speed,0);
*/
		out = ButtonInput();
      speed = 5;
      if (out == " ON")
         CtrlDcMotor(speed, 0);
      else if (out == "OFF")
         CtrlDcMotor(speed, 2);

      ms_delay(100); // 힌트 : 빠른속도로 Display를 하게되면 문제가 발생한다.
   }



#elif Experiment == 2

   unsigned int speed=0;
   unsigned int out=0;
   unsigned int butten_check=0;

   initPort();      // 입출력 포트 초기화

    LCD_init();     // LCD 초기화

   initMotor();   // 스텝모터 제어를 위한 타이머/카운터 초기화 

   sei();         // 사용이유에 대해서 보고서에 첨부하기바람.

   LCD_cmd(0x01);   // LCD 화면 지우기 
   ms_delay(50);   // 50ms 지연 

   LCD_Write(0,0, table1);   // 초기 글자 출력
   LCD_Write(0,1, table2);

   ms_delay(1500);
   //LCD_cmd(0x01);         // LCD 화면 지우기 
   ms_delay(100);
 
   unsigned char *button;
   unsigned char *state;
   unsigned int button_check=0;
   while(1)
   {   
      /*****************************************
       실험2 코드작성
      : CPU보드의 버튼 S2(+)와 S3(-)를 입력 받음    
      : ButtonInput()함수를 이용하세요.
      : CtrlDcMotor()함수를 이용하세요.
      : DcMotorSpeedControl()함수를 이용하세요.
      ******************************************/
     
    /*
    if ((out=="SPEED_UP"||out=="SPEED_DOWN")&&butten_check==0) 
    {
    butten_check=1;
    
     speed = DcMotorSpeedControl(out, speed);
    }
    if (out=="SPEED_NO_CHANGE"&&butten_check==1) butten_check=0;
      */
   
   button = ButtonInput();

   if (button != state) button_check = 0;

   if (button_check == 0)
   {
      speed = DcMotorSpeedControl(button, speed);
      button_check = 1;
	  state = button;
   }

   CtrlDcMotor(speed,0);
     
    char temp[2];
     sprintf(temp,"%d",speed);
     LCD_Write(13,1, temp);



      ms_delay(100);
   }

#endif
}
