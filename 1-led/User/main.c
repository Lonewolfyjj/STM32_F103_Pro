//#include "stm32f10x.h"
//#include "delay.h"
//#include "led.h"

//int main(void)
//{

//	delay_init();
//	LED_Init();

//	while(1){
//		
//		GPIO_WriteBit(GPIOB,LED_BLUE,Bit_SET);
//		GPIO_WriteBit(GPIOE,LED_RED,Bit_SET);
//		
//		delay_ms(500);
//		GPIO_WriteBit(GPIOB,LED_BLUE,Bit_RESET);
//		GPIO_WriteBit(GPIOE,LED_RED,Bit_RESET);
//		
//		delay_ms(500);
//	}
//	
//	
//}

#include "reg.h"
#include "key.h"
#include "delay.h"
#include "led.h"
void delay(void);
void led_init(void);

int main()
{
	unsigned short keyres;
	RCC_APB2ENR |= 1<<3; // 使能PB 模块的时钟				
	RCC_APB2ENR |= 1<<6; // 使能PE 模块的时钟			
	delay_init();
	led_init();
	key_init();
	
	while(1)							
	{
		keyres = key_scan();
		if(keyres == KEY0_PRES) {
			GPIOB_ODR |= 1<<5; 
			GPIOE_ODR &= ~(1<<5);
			//delay_ms(200);
		} 
		if (keyres == KEY1_PRES){
			GPIOB_ODR &= ~(1<<5);
			GPIOE_ODR |= 1<<5; 
			//delay_ms(200);
		}
	}
}

void led_init(void) 
{
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
	GPIOB_ODR &= ~(1<<5); 
  GPIOE_ODR &= ~(1<<5); 

}
