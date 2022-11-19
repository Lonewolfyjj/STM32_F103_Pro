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
	
    delay_init();	    	 //延时函数初始化	
    uart_init(115200);
    init_lcd();
    delay_ms(100);
    ILI9341_Clear(0,0,LCD_X_LENGTH, LCD_Y_LENGTH);
    ILI9341_DispString_EN_CH(0,0,"中华人民共和国");

    ILI9341_DispString_EN_CH(0,33,"hello !!!");
    
	while(1){  
        
        testCNT++;
        testFloat+=0.1;
        delay_ms(1000);
          /********显示变量示例*******/
        LCD_SetTextColor(GREEN);
        /*使用c标准库把变量转化成字符串*/
        sprintf(dispBuff,"显示变量： %d",testCNT);
        /*然后显示该字符串即可，其它变量也是这样处理*/
        ILI9341_DispString_EN_CH(0,70,dispBuff);
        
        LCD_SetTextColor(GREEN);
        /*使用c标准库把变量转化成字符串*/
        sprintf(dispBuff,"浮点： %.2f",testFloat);
        /*然后显示该字符串即可，其它变量也是这样处理*/
        ILI9341_DispString_EN_CH(0,110,dispBuff);
    }
}


