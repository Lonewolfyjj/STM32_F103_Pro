#include "encoder.h"
#include "led.h"

void Init_Encoder()
{
  // GPIO�������ų�ʼ��
	
	Init_GPIO();
	// ��ʱ����ʼ��
	Timer2_Init();
	
}

void Init_GPIO()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void Timer2_Init()
{
	// ʹ��TIM2���ص�����
	RCC->APB1ENR |= 1<<0;
	
	/*ʱ����Ԫ����*/
	TIM2->CR1 |= 0X0081;  // ʹ�ܶ������������ü��������ϼ��������ض���ģʽ
	TIM2->ARR = 65536 - 1; // �����̼���
  TIM2->PSC = 1 - 1; //  �����l�����a��ʱ��ֱ��������ʱ��

	/*�������ӿ�����*/
	TIM2->SMCR |= 0x3; // �����ڴ�ģʽ�ı�����ģʽ3
	TIM2->CCMR1 |= 0x01;// IC1FP1������ͨ��1
	TIM2->CCMR2 |= 0x01;// IC2FP2������ͨ��2
	
  // CC1����ͨ��ʹ�ܣ����벻����CC2 ����ͨ��ʹ�ܣ����벻����
	TIM2->CCER &= 0xFF00;
	TIM2->CCER |= 0x0011;
	
	TIM2->CR1 |= 0x01; // ʹ�ܼ�����
	
  //TIM2->DIER |= 1<<0; // �����ж�ʹ��
	//Init_NVIC(TIM2_IRQn);
} 

int16_t getEncoder() 
{
	int16_t temp;
	temp = TIM2->CNT;
	TIM2->CNT = 0;
  return temp;

}
//void TIM2_IRQHandler() 
//{
//  if(TIM2->SR & 0x0001){
//	  LED_RED = ~LED_RED;
//		TIM3->SR &= ~(1<<0);
//	}
//}


//void Init_NVIC(u8 interrupt_channel) {

//	unsigned char group = 4;
//	unsigned char priority = 0x7;
//	volatile unsigned int temp;
//	
//  // ��ʼ���жϱ�
//  // �������EXIT4�жϡ�����ICER[channel/32] |= 1<<(channel%32) �Ĵ�����д���Ӧλ1;
//	// ��ARIC ��������ռ���ȼ�����Ӧ���ȼ� 
//	temp = SCB->AIRCR; // ��ȡ��ǰ��ֵ
//	temp &= 0X0000F8FF;  // ��ո�λ������BIT[10��8] ��0
//  temp |= (0X05FA<<16 | (7 - group) << 8);  // ������Կ���������ȼ����÷���3
//	SCB->AIRCR = temp;
//	
//  // 5.�����жϷ���(nvic) ��ʹ���ж�
//	// NVIC_ISERx  channel = 9 �����ⲿ�ж�4
//	NVIC->ISER[interrupt_channel/32] |= (1<<(interrupt_channel%32)); // ʹ��EXIT4 �ж�λ
//  NVIC->IP[interrupt_channel] = priority << 4;  //IPn[7:4] ��ʾ���ȼ��� ��ǰ�ж����ȼ�0111  ��ʾ ��ռ3����Ӧ1
//}
