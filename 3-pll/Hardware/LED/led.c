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


//void led_init(void) 
//{
//  // 设置PB5的引脚输入模式 为输出功能
//	GPIOB_CRL &= ~(3<<(11*2)); // 寄存器清零
//	GPIOB_CRL |= 1<<(11*2);   // 通用开漏输出
//  // 设置PE5的引脚输入模式 为输出功能
//	GPIOE_CRL &= ~(3<<(11*2)); // 寄存器清零
//	GPIOE_CRL |= 1<<(11*2);   // 通用开漏输出
//	
//	
//	// PB5的引脚速度设置为2M
//	GPIOB_CRL &= ~(3<<(10*2));
//	GPIOB_CRL |= 3<<(10*2);
//	// PE5的引脚速度设置为2M
//	GPIOE_CRL &= ~(3<<(10*2));
//	GPIOE_CRL |= 3<<(10*2);
//	
//  // 设置PB5引脚输出高电平，灭灯
//	GPIOB_ODR |= 1<<5; 
//  GPIOE_ODR |= 1<<5; 

//}

