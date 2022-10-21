#include "led.h"
#include "reg.h"
#include "typedef.h"
#include "sys.h"

void led_init(void) 
{
	
  RCC_APB2ENR |= 1<<3; // ʹ��PB ģ���ʱ��				
	RCC_APB2ENR |= 1<<6; // ʹ��PE ģ���ʱ��		
	
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
	GPIOB_ODR |= (1<<5); 
  GPIOE_ODR |= (1<<5); 

	
}

