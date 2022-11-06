#include "led.h"
#include "reg.h"
#include "typedef.h"
#include "sys.h"

void led_init(void) 
{
	
  RCC_APB2ENR |= 1<<3; // 使能PB5 模块的时钟				
	RCC_APB2ENR |= 1<<6; // 使能PE5 模块的时钟		
	
 // 设置PB5的引脚输入模式 为输出功能
	GPIOB1->CRL &= ~(3<<(10*2)); // 寄存器清零
	GPIOB1->CRL |= 1<<(10*2);   // 通用开漏输出
  // 设置PE5的引脚输入模式 为输出功能
	GPIOE1->CRL &= ~(3<<(10*2)); // 寄存器清零
	GPIOE1->CRL |= 1<<(10*2);   // 通用开漏输出
	
	
	// PB5的引脚速度设置为2M
	GPIOB1->CRL &= ~(3<<(10*2));
	GPIOB1->CRL |= 3<<(10*2);
	// PE5的引脚速度设置为2M
	GPIOE1->CRL &= ~(3<<(10*2));
	GPIOE1->CRL |= 3<<(10*2);
	
  // 设置PB5引脚输出高电平，灭灯
	GPIOB1->ODR |= (1<<5); 
  GPIOE1->ODR |= (1<<5); 
	
	//LED_RED = 1;
	//LED_GREEM = 1;
}

