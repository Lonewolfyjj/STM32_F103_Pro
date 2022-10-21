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
	RCC_APB2ENR |= 1<<3; // ʹ��PB ģ���ʱ��				
	RCC_APB2ENR |= 1<<6; // ʹ��PE ģ���ʱ��			
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
  // ����PB5����������ģʽ Ϊ�������
	GPIOB_CRL &= ~(3<<(10*2)); // �Ĵ�������
	GPIOB_CRL |= 1<<(10*2);   // ͨ�ÿ�©���
  // ����PE5����������ģʽ Ϊ�������
	GPIOE_CRL &= ~(3<<(10*2)); // �Ĵ�������
	GPIOE_CRL |= 1<<(10*2);   // ͨ�ÿ�©���
	
	
	// PB5�������ٶ�����Ϊ2M
	GPIOB_CRL &= ~(3<<(10*2));
	GPIOB_CRL |= 3<<(10*2);
	// PE5�������ٶ�����Ϊ2M
	GPIOE_CRL &= ~(3<<(10*2));
	GPIOE_CRL |= 3<<(10*2);
	
  // ����PB5��������ߵ�ƽ�����
	GPIOB_ODR &= ~(1<<5); 
  GPIOE_ODR &= ~(1<<5); 

}
