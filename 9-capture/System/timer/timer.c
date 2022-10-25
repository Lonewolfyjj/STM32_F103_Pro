#include "sys.h"
#include "timer.h"
#include "beep.h"
#include "led.h"
#include "delay.h"
#include "usart.h"

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

//TIM3 ͨ��1 ����ӳ��CH1/PB4 ��ΪPWM ����������
void timer3_init_input_capture() 
{
	
	// ʹ��TIM3��APB1��ʱ��
	RCC->APB1ENR|=1<<1; 	//TIM3ʱ��ʹ��    
	RCC->APB2ENR|=1<<4;    	//ʹ��PORT PC6ʱ��	
	GPIOC->CRL&=0XF0FFFFFF;	//PC6���
	GPIOC->CRL|=0X08000000;	//���ù������ 	  	 
  GPIOC->ODR|=0<<0;	    	//    PC6 ����

	
  TIM3->EGR |= 1<<0; //��ʼ�����еļĴ���
	
  TIM3->PSC = 7200-1;  // �õ� 10khz�ķ�Ƶ
	TIM3->ARR = 10-1;     // 1000HZ
	
  TIM3->CCMR1|=1<<0;            //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM3->CCMR1 &= ~(0xf<<4); 		//IC1F=0000 ���������˲��� ���˲�
	TIM3->CCMR1 &= ~(3<<2);       //IC2PS=00 	���������Ƶ,����Ƶ 
	
	TIM3->CCER &= ~(1<<1); 		//CC1P=0	�����ز���
	TIM3->CCER |= 1<<0; 		  //CC1E=1 	�������������ֵ������Ĵ�����

	TIM3->DIER|=1<<1;   	//�������ж�				
	TIM3->DIER|=1<<0;   	//��������ж�	
	TIM3->CR1|=0x01;    	//ʹ�ܶ�ʱ��3
	
	
	init_my_nvic(TIM3_IRQn);
	// ��TIM2���PWM��ͨ��������TIM3 CH1 ͨ���Ͻ��в���
}


// ��ʼ����TIM2 CH2 ͨ�� PWM1ģʽ, ��ӳ�䵽CH2/PB3���ţ���ΪTIM3 PB4���ŵ�PWM��������
void init_timer2_pwd() {
	
  /*ԭ�����ӣ�https://blog.csdn.net/qq_16963231/article/details/120069444*/
  GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);                 
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //ʹ��SWD ����JTAG
  //�˲������ֶ��޸�IO������
	RCC->APB1ENR|=1<<0; 	//TIM2ʱ��ʹ��    
	RCC->APB2ENR|=1<<3;    	//ʹ��PORTBʱ��	
	GPIOB->CRL&=0XFFFF0FFF;	//PB3���
	GPIOB->CRL|=0X0000B000;	//���ù������ 		  	 
	GPIOB->ODR|=1<<3; //PB3����Ϊ���
	
	RCC->APB2ENR|=1<<0;     //��������ʱ��	   
	AFIO->MAPR &= ~(3<<8); //���MAPR��[9:8]
	AFIO->MAPR |= 1<<8;      //������ӳ��,TIM2_CH2->PB3
	
	
	TIM2->EGR |= 1<<0; //��ʼ�����еļĴ���
	
  TIM2->PSC = 7200-1;  // �õ� 10khz�ķ�Ƶ
	TIM2->ARR = 1000-1;     // 1KZH 
	
  //��TIMx_CCMRx�Ĵ����е�OCxMλд�롯110��(PWMģʽ1)��111��(PWMģʽ2)���ܹ�����������ÿ��OCx���ͨ������һ·PWM
	TIM2->CCMR1 |= 6<<12; // ͨ��2 PWMģʽ������Ƚ�1ģʽ��
	TIM2->CCMR1 |= 1<<11;  // ����Ƚ�2Ԥװ��ʹ�� 
	TIM2->CCMR1 |= 1<<3;  // ����TIMx_CCR1�Ĵ�����Ԥװ�ع���(�Ĵ���) ��д��������Ԥװ�ؼĴ��������� TIMx_CCR1��Ԥװ��ֵ�ڸ����¼�����ʱ����������ǰ�Ĵ����С�
  TIM2->CCMR1 &= ~(3<<8); //CC2ͨ��������Ϊ���	
	
	// ʹ�ܶ������������ü��������ϼ��������ض���ģʽ
	TIM2->CR1 |= 0X0081; 	
	TIM2->CCER |= 1<<4; //OC2 ���ʹ��
	TIM2->CCER &= ~(1<<5);// OC2�ߵ�ƽ��Ч

  // �޸����ֵ5000�����AAR�Ĵ�����װ��ֵ10000������1hz�� 50%ռ�ձ�PWM
  TIM2->CCR2 = 500; // PB3����ͨ��2������CCR2 : 50% 1HZ PWM

}


u32 over_flow;
u32 START_VAL,END_VAL;
u16 count,edge;

float measureFrency(void);  // ����Ƶ��
float measureHight(void) ; // �����ߵ�ƽʱ��

void TIM3_IRQHandler(void)
{
	  float hight,temH;
	  float frency,temF;
	  static u8 pro;
		if(TIM3->SR & 0X0001) { // �����һ��
			over_flow++;
		}

		if(TIM3->SR & 0x0002){ // ����ȽϼĴ����Ѿ�����������Ԥ��Ƶ�������أ���������ֵ������CCR1��
			measureHight();
		}

	  TIM3->SR=0;//����жϱ�־λ 	 
}

float measureHight() 
{
	static float result;
	++edge;
	// ��һ��������ȡֵ
	if(edge == 1) {
		START_VAL = TIM3->CCR1;
		over_flow=0;
	  TIM3->CCER |= 1<<1; 		//CC1P=0	�½��ز���
		
	}
	
	// ��һ���½���ȡֵ,һ�������ĸߵ�ƽʱ��
	if(edge == 2) {
  
		edge = 0;
		END_VAL = TIM3->CCR1;
	  TIM3->CCER &= ~(1<<1); 		//CC1P=0	�����ز���
		count = over_flow;
		result = count * 0.001 + START_VAL - END_VAL;
		printf("�ߵ�ƽʱ�� ====== %f s\r\n",result);
		return result;
	}
	return 0;
}

float measureFrency() {
	static float result;
	++edge;
	if(edge == 1) {
		// ��һ��������ȡֵ
		START_VAL = TIM3->CCR1;
		over_flow=0;
	} 
	if(edge ==2)
	{
	// �ڶ���������ȡֵ��һ������������
		END_VAL = TIM3->CCR1;
		count = over_flow;
		edge = 0;
	  result = count * 0.001 + END_VAL - START_VAL;
		printf("Ƶ�� ====== %f Hz\r\n",result);
		return result;
	}
	return 0;

}

