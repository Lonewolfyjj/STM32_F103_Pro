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
	LED_Init();
	key_init();

	while(1)							
	{
		keyres = key_scan();
		if(keyres == KEY0_PRES) {
			GPIOB_ODR |= 1<<5; 
			GPIOE_ODR &= ~(1<<5);
		} 
		if (keyres == KEY1_PRES){
			GPIOB_ODR &= ~(1<<5);
			GPIOE_ODR |= 1<<5; 
		}
	}
}


