#include "reg.h"
#include "key.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "typedef.h"
#include "string.h"
#include "timer.h"
#include "encoder.h"
#include "usart.h"
#include <math.h>
#include "gui.h"

int main()
{
    u8 index=1,key=0;
	delay_init();
    beep_init();
	led_init();
	key_init();
	usart_init(72,115200);
    RTC_Init();
    OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
    OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	
	while(1)
	{
        OLED_Refresh();
       //OLED_Clear();
        GUI_Refresh();
//        OLED_ShowNum(35,24,2022,4,16, 0);
//        OLED_ShowNum(67,24,11,2,16, 1);
        //OLED_ShowNum(43,24,11,2,16, 1);
      
	}
}





















