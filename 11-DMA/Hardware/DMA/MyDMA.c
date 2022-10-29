#include "MyDMA.h"
#include "delay.h"

u16 DMA1_MEM_LEN;//����DMAÿ�����ݴ��͵ĳ��� 	

void DMA_init(u32 fromAddr,u32 toAddr,u16 count) 
{
	RCC->AHBENR|=1<<0;			//����DMA1ʱ��
	delay_ms(5);				    //�ȴ�DMAʱ���ȶ�
	
	DMA1_Channel1->CMAR = (u32)fromAddr;
	DMA1_Channel1->CPAR = (u32)toAddr;
	DMA1_Channel1->CNDTR = count; // DMA��������� N ��֮�����װѭ��
	DMA1_MEM_LEN = count;
	DMA1_Channel1->CCR=0X00000000;	//��λ
	DMA1_Channel1->CCR |= 1<<14; // �����洢�����洢��ģʽ
	DMA1_Channel1->CCR |= 3<<12; // ͨ�����ȼ����
	DMA1_Channel1->CCR &= ~(3<<10); // �洢�����ݿ�� 8λ
	DMA1_Channel1->CCR &= ~(3<<8); // �������ݿ�� 8bit
	
	DMA1_Channel1->CCR |= 1<<7; // �洢����ַ����ģʽ ����
	DMA1_Channel1->CCR |= 1<<6; // �����ַ����ģʽ ����
	DMA1_Channel1->CCR |= (1<<5); // ����ѭ��ģʽ
	
	DMA1_Channel1->CCR |= 1<<4; // ���ݴ��䷽�򣬴Ӵ洢����
	
  DMA1_Channel1->CCR |= 1<<0; // ��������
}

/**
   �����ʼ�� ��ִ�е���ѭ������Ҫ�ֶ�����һ�����ݴ���
*/ 
void start_Transt() 
{
  DMA1_Channel1->CCR &= ~(1<<0); // ��ֹ����
  DMA1_Channel1->CNDTR = DMA1_MEM_LEN;
	DMA1_Channel1->CCR |= 1<<0; // ��������
}
