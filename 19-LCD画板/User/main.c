#include "ili9341.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include <stdio.h>
#include "xpt2046_lcd.h"

extern uint8_t LCD_SCAN_MODE;

int main()
{
    static uint8_t testCNT = 0;	
    static float testFloat = 1.0;
    char dispBuff[100];
    uint8_t num;
	uint16_t recvDat;
    delay_init();	    	 //��ʱ������ʼ��	
    uart_init(115200);
    init_lcd();
    LED_Init();
    
    
    
//    delay_ms(100);
//    ILI9341_Clear(0,0,LCD_X_LENGTH, LCD_Y_LENGTH);
//    ILI9341_DispString_EN_CH(0,0,"�л����񹲺͹�");

//    ILI9341_DispString_EN_CH(0,33,"hello !!!");
    
    XPT2046_Init();
    //��FLASH���ȡУ����������FLASH�޲�������ʹ��ģʽ3����У��
	Calibrate_or_Get_TouchParaWithFlash(LCD_SCAN_MODE,0);
    printf("LCD_SCAN_MODE = %d\r\n",LCD_SCAN_MODE);
    Palette_Init(LCD_SCAN_MODE);
    
	while(1){  
         printf("XPT2046_PENIRQ_Read() = %d\r\n",XPT2046_PENIRQ_Read());
		XPT2046_TouchEvenHandler();
    }
}


