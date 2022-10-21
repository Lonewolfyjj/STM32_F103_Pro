#include "led.h"
#include "stm32f10x.h"
#include "reg.h"

void LED_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	// ʹ��PB �˿ڵ�APB2
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
//  // ����PB5����������ģʽ Ϊ�������
//	GPIOB_CRL &= ~(3<<(11*2)); // �Ĵ�������
//	GPIOB_CRL |= 1<<(11*2);   // ͨ�ÿ�©���
//  // ����PE5����������ģʽ Ϊ�������
//	GPIOE_CRL &= ~(3<<(11*2)); // �Ĵ�������
//	GPIOE_CRL |= 1<<(11*2);   // ͨ�ÿ�©���
//	
//	
//	// PB5�������ٶ�����Ϊ2M
//	GPIOB_CRL &= ~(3<<(10*2));
//	GPIOB_CRL |= 3<<(10*2);
//	// PE5�������ٶ�����Ϊ2M
//	GPIOE_CRL &= ~(3<<(10*2));
//	GPIOE_CRL |= 3<<(10*2);
//	
//  // ����PB5��������ߵ�ƽ�����
//	GPIOB_ODR |= 1<<5; 
//  GPIOE_ODR |= 1<<5; 

//}

