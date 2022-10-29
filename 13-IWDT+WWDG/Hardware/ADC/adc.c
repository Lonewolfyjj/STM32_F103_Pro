#include "adc.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

u16 adcRes;
u16 adbde = 123;
void init_adc() 
{
	RCC->APB2ENR |= 1<<9; 	// ����ADC1 RCCʱ��ʹ��
  RCC->APB2ENR |= 1<<2;  // ���ö�Ӧͨ������GPIOA���ŵ�ʱ��ʹ��
  RCC->CFGR &= ~(3<<14); 
	RCC->CFGR |= 2<<14; // ����ADC CLK ��ʱ��Ƶ��
	
  RCC->AHBENR|=1<<0;			//����DMA1ʱ��
	
	RCC->APB2RSTR |= 1<<9; // ADC1�ӿڸ�λ
	RCC->APB2RSTR &= ~(1<<9);//��λ����	 
	
  // ���ö�ӦGPIO ͨ�����ŵ�����ģ��ģʽ(adcר��ģʽ)
	GPIOA->CRL &= ~(0XF<<4);
	
	ADC1->CR1 &=  ~(0XF<<16); //����ADC�����ڶ���ģʽ
	
	ADC1->CR1 &= ~(1<<8); // �ر�ɨ��ģʽ	
  ADC1->CR2 |= 1<<1; // ѭ��ת��ģʽ   ����ת����ɨ��ģʽ
	
	ADC1->CR2 |= (1<<22);
  ADC1->CR2 &= ~(1<<20);      //ʹ�����ⲿ����(SWSTART)!!!	����ʹ��һ���¼�������
	ADC1->CR2 |= 7<<17; //ѡ����������ͨ����ת�����ⲿ�¼�������ѡ���������

	ADC1->CR2 &= ~(1<<11); //ѡ�������Ҷ���
	
	ADC1->CR2 |= (1<8); //ʹ��DMAģʽ
	
  ADC1->SMPR2 |= 	7<<3; //ADC ͨ��1 ����239.5����ȷ�����ȣ���ʧ����
	
	ADC1->SQR1&=~(0XF<<20); //����ͨ�����г��ȶ���Ϊ1�����Թ���ͨ����ֻ��һ��AD���ݽ���ת���� ��Ϊ��ǰ����ֻ����PA1ͨ�������ݣ���������Ϊ1 �������Ҫ���ͨ�����ת������Ҫ�ڼӶ������ͨ�����൱��ÿ���ܲ�Ҫ��������ﳤ��ADC ������ת��Ҳ��Ҫ���������ͨ�������ת����
  
	ADC1->SQR3 &= 0XFFFFFFE0; // ��������1 ͨ�����
	ADC1->SQR3 |= ADC_CH1; 			// ͨ��1(������PA1)	; // PA1��Ӧͨ��1		  
	
  ADC1->CR2 |= 1<<0; // ����ADC������ת��
	
  ADC1->CR2 |= 1<<3; // ��ʼ��λУ׼
	while(ADC1->CR2&1<<3); // ��λУ׼���
	ADC1->CR2 |= 1<<2; // ��ʼУ׼
	while(ADC1->CR2 & 1<<2); // У׼���
  ADC1->CR2 |= 1<<22; // ADC��ʼת������ͨ��
	
	/****************DMA init********************/
	delay_ms(5);				    //�ȴ�DMAʱ���ȶ�
	
	DMA1_Channel1->CMAR = (uint32_t)&ADC1->DR;
	DMA1_Channel1->CPAR = (uint32_t)&adcRes;
	DMA1_Channel1->CNDTR = 1; // DMA��������� N ��֮�����װѭ��

	DMA1_Channel1->CCR=0X00000000;	//��λ
	DMA1_Channel1->CCR |= 1<<14; // �����洢�����洢��ģʽ
	DMA1_Channel1->CCR |= 3<<12; // ͨ�����ȼ����
	DMA1_Channel1->CCR |= 1<<10; // �洢�����ݿ�� 16λ
	DMA1_Channel1->CCR |= 1<<8; // �������ݿ�� 16bit
	
	DMA1_Channel1->CCR |= 1<<7; // �洢����ַ����ģʽ ����
	DMA1_Channel1->CCR |= 1<<6; // �����ַ����ģʽ ����
	DMA1_Channel1->CCR |= (1<<5); // ����ѭ��ģʽ
	
	DMA1_Channel1->CCR |= 1<<4; // ���ݴ��䷽�򣬴Ӵ洢����
	
  DMA1_Channel1->CCR |= 1<<0; // ��������

} 

uint16_t getADC() 
{

	while(!(ADC1->DR & 1<<1));
  // ����1�������飬ѭ��ת��ģʽ��ֱ�Ӷ�ȡ���ݾ������� 
	return ADC1->DR;
}


u16 Get_Adc_Average()
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<10;t++)
	{
		temp_val+=getADC();
		delay_ms(5);
	}
	return temp_val/10;
} 
	 
