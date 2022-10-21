#include "led.h"
#include "reg.h"
#include "typedef.h"
#include "sys.h"

void led_init(void) 
{
	
  RCC_APB2ENR |= 1<<3; // 使能PB 模块的时钟				
	RCC_APB2ENR |= 1<<6; // 使能PE 模块的时钟		
	
 // 设置PB5的引脚输入模式 为输出功能
	GPIOB_CRL &= ~(3<<(10*2)); // 寄存器清零
	GPIOB_CRL |= 1<<(10*2);   // 通用开漏输出
  // 设置PE5的引脚输入模式 为输出功能
	GPIOE_CRL &= ~(3<<(10*2)); // 寄存器清零
	GPIOE_CRL |= 1<<(10*2);   // 通用开漏输出
	
	
	// PB5的引脚速度设置为2M
	GPIOB_CRL &= ~(3<<(10*2));
	GPIOB_CRL |= 3<<(10*2);
	// PE5的引脚速度设置为2M
	GPIOE_CRL &= ~(3<<(10*2));
	GPIOE_CRL |= 3<<(10*2);
	
  // 设置PB5引脚输出高电平，灭灯
	GPIOB_ODR |= (1<<5); 
  GPIOE_ODR |= (1<<5); 

	
}

