#include "ili9341.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include <stdio.h>
#include "w25q128.h"

int main()
{
    LED_Init();
    delay_init();	    	 //延时函数初始化	
    uart_init(115200);
    init_lcd();
    delay_ms(100);
    W25Q128_Init();
    XPT2046_Init();
    
	rtp_test();
}


