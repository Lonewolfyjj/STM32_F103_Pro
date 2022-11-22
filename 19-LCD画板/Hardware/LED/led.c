#include "led.h"
#include "stm32f10x.h"
#include "reg.h"

void LED_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	// 使能PB 端口的APB2
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//GPIOB
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);//GPIOE
	
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_5);
}

void led_red_toggle() 
{
	LED_RED_ON;
    delay_ms(100);
    LED_RED_OFF;
    delay_ms(100);

}

void led_green_toggle() 
{
	LED_GREEN_ON;
    delay_ms(100);
    LED_GREEN_OFF;
    delay_ms(100);

}
