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
#include <math.h>   

#include "lcdControl.h"
#include "myCANLib.h"
#include "myCANLib.c"

#include "CAN_NODE_2_AUTO.c"
#include "CAN_NODE_2_AUTO.h"
#include "CAN_NODE_2_AUTO_private.h"
#include "CAN_NODE_2_AUTO_types.h"
#include "rtwtypes.h"


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


char *mode[4][12] = { "WAIT", "ACCR", "SLOW", "INIT"};

unsigned char strBuff[20]={0};

unsigned char distenceCheck=1;

struct MOb msg1;/////////////
struct MOb msg3 = {3, 0x03, EXT, 8, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};////////////

int current_mode;
int current_speed;

float distance2;
int gyro_x;
int speed;
int distance_check;



volatile static unsigned char buf[10];
volatile unsigned int ch0, ch2, ch4;
volatile unsigned int data0[10], data2[10], data4[10];
volatile unsigned int sum0, sum2, sum4;
volatile unsigned int Final0, Final2, Final4;
volatile unsigned int i, FinalX;
volatile unsigned char str[10];

void Sort0(void);
void Sort2(void);
void Sort4(void);
void Swap(volatile unsigned int *a, volatile unsigned int *b); 

void get_adc_data(void);
void sys_init(void);

void Sort0(void) 
{
  int i, j;
  for (i=0; i<10; i++) {
       for (j=i+1; j<11; j++) { 
            if (data0[i] > data0[j]) Swap(&data0[i], &data0[j]);
     }
  }
} 

void Sort2(void) 
{
  int i, j;
  for (i=0; i<10; i++) {
       for (j=i+1; j<11; j++) { 
            if (data2[i] > data2[j]) Swap(&data2[i], &data2[j]);
     }
  }
} 

void Sort4(void) 
{
  int i, j;
  for (i=0; i<10; i++) {
       for (j=i+1; j<11; j++) { 
            if (data4[i] > data4[j]) Swap(&data4[i], &data4[j]);
     }
  }
} 


void Swap(volatile unsigned int *a, volatile unsigned int *b) 
{
  volatile unsigned int tmp=*a;
  *a=*b;
  *b=tmp;
}

void sys_init(void){
    int i;
    ch0 = 0;
    ch2 = 0;
    ch4 = 0;
    
    for(i=0;i<10;i++){
       buf[i] = 0;
      data0[i] = 0;
      data2[i] = 0;
      data4[i] = 0;
    }    
}

void get_adc_data(void)
{
   unsigned int i;
   
   ch0 = ch2 = ch4 = 0;
   for(i=0;i<16;i++){
//X
      ADCSRA &= 0x7f;
      ADMUX = 0x40;
      ADCSRA |= 0xC0;

      while((ADCSRA & 0x10) != 0x10);
      buf[3] = ADCL;
      buf[2] = ADCH & 0x3f;
      ch0 = buf[3]+buf[2]*256;
//Y
      ADCSRA &= 0x7f;
      ADMUX = 0x42;
      ADCSRA |= 0xC0;

      while((ADCSRA & 0x10) != 0x10);
      buf[5] = ADCL;
      buf[4] = ADCH & 0x3f;
      ch2 = buf[5]+buf[4]*256;
//Z      
      ADCSRA &= 0x7f;
      ADMUX = 0x44;
      ADCSRA |= 0xC0;
      
      while((ADCSRA & 0x10) != 0x10);
      buf[7] = ADCL;
      buf[6] = ADCH & 0x3f;
      ch4 = buf[7]+buf[6]*256;      
   }      
}

int Viking(void) {
          
      for(i=0;i<10;i++){
         get_adc_data();
         data0[i] = ch0;
         data2[i] = ch2;
         data4[i] = ch4;         
      }      
      // Sort
      Sort0();
      Sort2();
      Sort4();
      
      sum0 = sum2 = sum4 = 0;
      
      for(i=2;i<9;i++){
         sum0 = sum0 + data0[i];
         sum2 = sum2 + data2[i];
         sum4 = sum4 + data4[i];         
      }

      Final0 = floor(sum0 / 8);
      Final2 = floor(sum2 / 8);
      Final4 = floor(sum4 / 8);
      
      FinalX = Final0 - 350;

      return (int)FinalX;
}






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

void rt_OneStep(void)
{
  static boolean_T OverrunFlag = 0;

  /* Disable interrupts here */

  /* Check for overrun */
  if (OverrunFlag) {
    rtmSetErrorStatus(CAN_NODE_2_AUTO_M, "Overrun");
    return;
  }

  OverrunFlag = TRUE;

  /* Save FPU context here (if necessary) */
  /* Re-enable timer or interrupt here */
  /* Set model inputs here */

  //CAN_NODE_2_AUTO_U.distance = (double)distance2;

  CAN_NODE_2_AUTO_U.gyro_x = (double)gyro_x;
  CAN_NODE_2_AUTO_U.msg1_data_0_ = msg1.data[0];
  CAN_NODE_2_AUTO_U.speed = speed;
  /* Step the model */
  CAN_NODE_2_AUTO_step();

  /* Get model outputs here */

  /* Outport: '<Root>/msg3_data_0_' */
  msg3.data[0] = CAN_NODE_2_AUTO_Y.msg3_data_0_;

  /* Outport: '<Root>/ocr3a' */
  OCR3A = CAN_NODE_2_AUTO_Y.ocr3a;

  /* Outport: '<Root>/ocr3b' */
  OCR3B = CAN_NODE_2_AUTO_Y.ocr3b;

  /* Outport: '<Root>/porte' */
  PORTE = CAN_NODE_2_AUTO_Y.porte;

  /* Outport: '<Root>/distance_check' */
  //distance_check = CAN_NODE_2_AUTO_Y.distance_check;
  /* Indicate task complete */
  OverrunFlag = FALSE;

  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}

int main(void)
{
   unsigned int speed=3;
   unsigned int mode = 0; //0은 wait, 1은 speed up, 2는 speed down
   unsigned char thousand, hundred, ten, one;
   float distance;
   unsigned char position = 0;
   unsigned char distenceCheck=0;
   unsigned int dir = 2;
   unsigned int accTime = 0;
   unsigned int tempTime = 0;
   char temp[6];
   
   unsigned int firstMove = 0;

	can_init(b250k   );   // CAN 보레이트를 원하는 값으로 세팅한다.

   unsigned char *button;
   can_init(5);       // 초기화
   can_init_8Mhz(5);
   can_rx_set(1, 0x01, EXT, 8, 0x00, 0x05  );       // CAN 수신기 초기화 

   ADMUX = 0x00;                                 // single-ended ADC3
   ADCSRA = 0x86;                                // ADC enable, 125kHz



   initPort();      // 입출력 포트 초기화

    LCD_init();     // LCD 초기화

   initMotor();   // 스텝모터 제어를 위한 타이머/카운터 초기화 

   sei();         // 사용이유에 대해서 보고서에 첨부하기바람.

   LCD_cmd(0x01);   // LCD 화면 지우기 
 
 // 디버그용 코드 
    speed = 3;
	//gyro_x = 40;
	distance_check = 0;
      
   while(1)
   {

	

 

  
	gyro_x = Viking();

   can_rx(1, &msg1);

   rt_OneStep();
  

   if (msg3.data[0] == 0) LCD_Write(0,0, "WAIT");//모드 출력
   if (msg3.data[0] == 1) LCD_Write(0,0, "SPUP");//모드 출력
   if (msg3.data[0] == 2) LCD_Write(0,0, "SPDN");//모드 출력
   if (msg3.data[0] == 4) LCD_Write(0,0, "INIT");//모드 출력

   sprintf(temp,"%d", gyro_x );
	LCD_Write(10,1, temp);

    sprintf(temp,"%d",distance_check);
    LCD_Write(0,1, temp);
    


  }


}
