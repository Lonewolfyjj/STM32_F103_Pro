#include "MyDMA.h"
#include "delay.h"

u8 data1[] = {0x01,0x02,0x03,0x04};
u8 data2[] = {123,123,123,123};

void DMA_init() 
{
	RCC->AHBENR|=1<<0;			//����DMA1ʱ��
	delay_ms(5);				//�ȴ�DMAʱ���ȶ�

	DMA1_Channel1->CCR |= 1<<14; // �����洢�����洢��ģʽ
	DMA1_Channel1->CCR |= 3<<12; // ͨ�����ȼ����
	DMA1_Channel1->CCR &= ~(3<<10); // �洢�����ݿ�� 8λ
	DMA1_Channel1->CCR &= ~(3<<8); // �������ݿ�� 8bit
	
	DMA1_Channel1->CCR |= 1<<7; // �洢����ַ����ģʽ ����
	DMA1_Channel1->CCR |= 1<<6; // �����ַ����ģʽ ����
	DMA1_Channel1->CCR |= (1<<5); // ����ѭ��ģʽ
	
	DMA1_Channel1->CCR |= 1<<4; // ���ݴ��䷽�򣬴Ӵ洢����
	
}


void start_Transt(u32 fromAddr,u32 toAddr,u16 count) 
{
	DMA1_Channel1->CMAR = (u32)fromAddr;
		
	DMA1_Channel1->CPAR = (u32)toAddr;
	
  DMA1_Channel1->CNDTR = count; // DMA��������� N ��֮�����װѭ��
	
	DMA1_Channel1->CCR |= 1<<0; // ��������

}
