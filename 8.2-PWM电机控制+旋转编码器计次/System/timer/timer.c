#include "sys.h"
#include "timer.h"
#include "beep.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
uint8_t duty_width;
void init_timer3(u16 psc, u16 arr) 
{

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

void init_timer3_pwd() 
{	
  //�˲������ֶ��޸�IO������
	RCC->APB1ENR|=1<<1; 	//TIM3ʱ��ʹ��    
	RCC->APB2ENR|=1<<3;    	//ʹ��PORTBʱ��	
	GPIOB->CRL&=0XFF0FFFFF;	//PB5���
	GPIOB->CRL|=0X00B00000;	//���ù������ 	  	 
	   
	RCC->APB2ENR|=1<<0;     //��������ʱ��	   
	AFIO->MAPR&=0XFFFFF3FF; //���MAPR��[11:10]
	AFIO->MAPR|=1<<11;      //������ӳ��,TIM3_CH2->PB5
	
	TIM3->EGR |= 1<<0; //��ʼ�����еļĴ���
	
  TIM3->PSC = 7200-1;  // �õ� 10khz�ķ�Ƶ
	TIM3->ARR = 100;     // ÿ�ε�100HZ.���½���װ�� ��10KZH --> 100HZ
	
  //��TIMx_CCMRx�Ĵ����е�OCxMλд�롯110��(PWMģʽ1)��111��(PWMģʽ2)���ܹ�����������ÿ��OCx���ͨ������һ·PWM
	TIM3->CCMR1 |= 6<<12; // ͨ��2 PWMģʽ������Ƚ�1ģʽ��
	TIM3->CCMR1 |= 1<<11;  // ����Ƚ�2Ԥװ��ʹ�� 
	TIM3->CCMR1 |= 1<<3;  // ����TIMx_CCR1�Ĵ�����Ԥװ�ع���(�Ĵ���) ��д��������Ԥװ�ؼĴ��������� TIMx_CCR1��Ԥװ��ֵ�ڸ����¼�����ʱ����������ǰ�Ĵ����С�
  TIM3->CCMR1 &= ~(2<<0); //CC1ͨ��������Ϊ���	
	
	// ʹ�ܶ������������ü��������ϼ��������ض���ģʽ
	TIM3->CR1 |= 0X0081; 	
	TIM3->CCER |= 1<<4; //OC2 ���ʹ��
	TIM3->CCER &= ~(1<<5);// OC2�ߵ�ƽ��Ч

  // �޸����ֵ5000�����AAR�Ĵ�����װ��ֵ10000������1hz�� 50%ռ�ձ�PWM
  TIM3->CCR2 = 50; // PB5����ͨ��2������CCR2 : 50% 1HZ PWM
	
  //TIM3->DIER |= 1<<0; // �����ж�ʹ��
 	//init_my_nvic(TIM3_IRQn);
}

void setPwmDuty(int32_t duty) 
{
	duty_width = duty % 100;
	if(duty >= 100) {
	  duty_width = 100;
		duty = 100;
	}
	else if(duty <=0)
	{
	   duty_width = 0;
		 duty = 0;
	}
	else 
	{
	  duty_width = duty;
	}
	printf("duty ======== %d\r\n",duty_width);
	TIM3->CCR2 = duty_width; 
}

void TIM3_IRQHandler(void)
{
	static u32 temp=0;
	static u8 b=2;
	if(TIM3->SR & 0x0001){
		
		if(b==2 && temp++ > 100) {
			temp = 100;
			b  = 1;
		} 
		
		if(b==1 && temp-- < 1) {
		  temp = 0;
			b = 2;
		}
		
		TIM3->CCR2 = temp; // �ı�CCR2ֵ�����Ըı�ռ�ձ�
		TIM3->SR &= ~(1<<0);
	}
	
}	

