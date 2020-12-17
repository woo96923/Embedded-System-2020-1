#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>	
		
#include "lcdControl.h"
#include "myDelay.h"
#include "rtwtypes.h"
#include "Timer.h"


#define BUTTON0_MASK 0x01
#define BUTTON1_MASK 0x02

unsigned char table1[16]="LCD_TEST";
unsigned char table2[16]="MODE : ";
unsigned char strBuff[20]={0};

int mode;
int switch1;

void initPort(void)
{
	DDRC  = 0xff;	// LCD 데이터 및 명령 
	PORTC = 0xff;
	DDRG  = 0xff;	// LCD 제어 출력 (RS, RW, E)
	DDRF  = 0xfc;	

	DDRE  = 0xff;

    DDRD  = 0x00;   // SW 입력설정

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
	    asm("PUSH R0"); 	// 2 cycle +
		asm("POP R0"); 		// 2 cycle +
		asm("PUSH R0"); 	// 2 cycle +
		asm("POP R0"); 		// 2 cycle + =12 cycle for 11.0592MHZ
		asm("PUSH R0"); 	// 2 cycle +
		asm("POP R0"); 		// 2 cycle = 16 cycle = 1us for 16MHz
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
  //extern ExternalInputs_Timer Timer_U;
  static boolean_T OverrunFlag = 0;

  /* Disable interrupts here */

  /* Check for overrun */
  if (OverrunFlag) {
    rtmSetErrorStatus(Timer_M, "Overrun");
    return;
  }

  OverrunFlag = TRUE;

  /* Save FPU context here (if necessary) */
  /* Re-enable timer or interrupt here */
  /* Set model inputs here */
  //Timer_U.In1 = (double)switch1;
  Timer_U.In1 = (double)switch1;

  /* Step the model */
  Timer_step();

  /* Get model outputs here */

  mode = Timer_Y.Out1;
  PORTE = Timer_Y.Out2;



  /* Indicate task complete */
  OverrunFlag = FALSE;

  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}


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

   if((up==1) && (down==0)) return 0;   // S2버튼 누른 경우
   if((up==0) && (down==1)) return 1;   // S3버튼 누른 경우

   return -1;
}



int main(void)
{


	
	initPort();				// 입출력 포트 초기화
   	LCD_init();     		// LCD 초기화
	LCD_cmd(0x01);	// LCD 화면 지우기 
	ms_delay(50);	// 50ms 지연 

	LCD_Write(0,0, table1);	// 초기 글자 출력
	LCD_Write(0,1, table2);
	ms_delay(200);

	

	sei();			// INT 인에이블 
 
 
	while(1)
	{	

		switch1=ButtonInput();
		rt_OneStep();
		LCD_Write(0,0, table1);	// 초기 글자 출력
		sprintf(strBuff,"mode : %04d", mode);
		LCD_Write(0,1, strBuff);
	}



}







