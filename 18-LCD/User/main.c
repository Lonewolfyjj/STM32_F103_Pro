#include "ili9341.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include <stdio.h>


int main()
{
    static uint8_t testCNT = 0;	
    static float testFloat = 1.0;
    char dispBuff[100];
    uint8_t num;
	
    delay_init();	    	 //��ʱ������ʼ��	
    uart_init(115200);
    init_lcd();
    delay_ms(100);
    ILI9341_Clear(0,0,LCD_X_LENGTH, LCD_Y_LENGTH);
    ILI9341_DispString_EN_CH(0,0,"�л����񹲺͹�");

    ILI9341_DispString_EN_CH(0,33,"hello !!!");
    
	while(1){  
        
        testCNT++;
        testFloat+=0.1;
        delay_ms(1000);
          /********��ʾ����ʾ��*******/
        LCD_SetTextColor(GREEN);
        /*ʹ��c��׼��ѱ���ת�����ַ���*/
        sprintf(dispBuff,"��ʾ������ %d",testCNT);
        /*Ȼ����ʾ���ַ������ɣ���������Ҳ����������*/
        ILI9341_DispString_EN_CH(0,70,dispBuff);
        
        LCD_SetTextColor(GREEN);
        /*ʹ��c��׼��ѱ���ת�����ַ���*/
        sprintf(dispBuff,"���㣺 %.2f",testFloat);
        /*Ȼ����ʾ���ַ������ɣ���������Ҳ����������*/
        ILI9341_DispString_EN_CH(0,110,dispBuff);
    }
}


