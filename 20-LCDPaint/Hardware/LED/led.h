#ifndef __LED_H
#define __LED_H

#define LED_RED_ON	    GPIO_ResetBits(GPIOB,GPIO_Pin_5)	//定义IO接口
#define LED_RED_OFF	    GPIO_SetBits(GPIOB,GPIO_Pin_5)	//定义IO接口

#define LED_GREEN_ON	    GPIO_ResetBits(GPIOE,GPIO_Pin_5)	//定义IO接口
#define LED_GREEN_OFF	    GPIO_SetBits(GPIOE,GPIO_Pin_5)	//定义IO接口


void LED_Init(void);
void led_green_toggle(void) ;
void led_red_toggle(void) ;

#endif


