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
	GPIO_InitStructure.GPIO_Pin=LED_RED;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,LED_RED);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin= LED_BLUE;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_SetBits(GPIOE,LED_BLUE);
}

void led_on() 
{
	GPIOB_ODR |= 1<<5; 
  GPIOE_ODR |= 1<<5; 

}

