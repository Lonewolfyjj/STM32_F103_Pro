#include "sys.h"
#include "timer.h"
#include "beep.h"
#include "led.h"
#include "delay.h"

void init_timer3(u16 psc, u16 arr) 
{
	// ����ʱ��Դ,�����ⲿ����ʱ��
	
	// ʹ��TIM3���ص�����
	RCC->APB1ENR |= 1<<1;
	// ʹ�ܶ������������ü��������ϼ��������ض���ģʽ
	TIM3->CR1 |= 0X0081; 
	TIM3->ARR = arr-1; 
	TIM3->PSC = psc-1;  // �õ� 10khz�ķ�Ƶ
  TIM3->DIER |= 1<<0; // �����ж�ʹ��
	init_my_nvic(TIM3_IRQn);
} 

void init_my_nvic(u8 interrupt_channel) {

	unsigned char group = 4;
	unsigned char priority = 0x7;
	volatile unsigned int temp;
	
  // ��ʼ���жϱ�
  // �������EXIT4�жϡ�����ICER[channel/32] |= 1<<(channel%32) �Ĵ�����д���Ӧλ1;
	// ��ARIC ��������ռ���ȼ�����Ӧ���ȼ� 
	temp = SCB->AIRCR; // ��ȡ��ǰ��ֵ
	temp &= 0X0000F8FF;  // ��ո�λ������BIT[10��8] ��0
  temp |= (0X05FA<<16 | (7 - group) << 8);  // ������Կ���������ȼ����÷���3
	SCB->AIRCR = temp;
	
  // 5.�����жϷ���(nvic) ��ʹ���ж�
	// NVIC_ISERx  channel = 9 �����ⲿ�ж�4
	NVIC->ISER[interrupt_channel/32] |= (1<<(interrupt_channel%32)); // ʹ��EXIT4 �ж�λ
  NVIC->IP[interrupt_channel] = priority << 4;  //IPn[7:4] ��ʾ���ȼ��� ��ǰ�ж����ȼ�0111  ��ʾ ��ռ3����Ӧ1
}

void TIM3_IRQHandler(void)
{
	if(TIM3->SR & 0x0001){
		LED_RED = ~LED_RED;
		TIM3->SR &= ~(1<<0);
	}
	
}	

