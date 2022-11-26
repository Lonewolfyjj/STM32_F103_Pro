#include "led.h"
#include "reg.h"
#include "typedef.h"
#include "sys.h"

void led_init(void) 
{
	
  RCC_APB2ENR |= 1<<3; // ʹ��PB5 ģ���ʱ��				
	RCC_APB2ENR |= 1<<6; // ʹ��PE5 ģ���ʱ��		
	
 // ����PB5����������ģʽ Ϊ�������
	GPIOB1->CRL &= ~(3<<(10*2)); // �Ĵ�������
	GPIOB1->CRL |= 1<<(10*2);   // ͨ�ÿ�©���
  // ����PE5����������ģʽ Ϊ�������
	GPIOE1->CRL &= ~(3<<(10*2)); // �Ĵ�������
	GPIOE1->CRL |= 1<<(10*2);   // ͨ�ÿ�©���
	
	
	// PB5�������ٶ�����Ϊ2M
	GPIOB1->CRL &= ~(3<<(10*2));
	GPIOB1->CRL |= 3<<(10*2);
	// PE5�������ٶ�����Ϊ2M
	GPIOE1->CRL &= ~(3<<(10*2));
	GPIOE1->CRL |= 3<<(10*2);
	
  // ����PB5��������ߵ�ƽ�����
	GPIOB1->ODR |= (1<<5); 
  GPIOE1->ODR |= (1<<5); 
	
	//LED_RED = 1;
	//LED_GREEM = 1;
}

