
#include "ili9341.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
int main()
{
    delay_init();	    	 //延时函数初始化	
    uart_init(115200);
    init_lcd();
    

    
    
	while(1){
     led_on() ;
    }
}


