#include "MyDMA.h"
#include "delay.h"

u16 DMA1_MEM_LEN;//保存DMA每次数据传送的长度 	

void DMA_init(u32 fromAddr,u32 toAddr,u16 count) 
{
	RCC->AHBENR|=1<<0;			//开启DMA1时钟
	delay_ms(5);				    //等待DMA时钟稳定
	
	DMA1_Channel1->CMAR = (u32)fromAddr;
	DMA1_Channel1->CPAR = (u32)toAddr;
	DMA1_Channel1->CNDTR = count; // DMA传输计数器 N 次之后就重装循环
	DMA1_MEM_LEN = count;
	DMA1_Channel1->CCR=0X00000000;	//复位
	DMA1_Channel1->CCR |= 1<<14; // 启动存储器到存储器模式
	DMA1_Channel1->CCR |= 3<<12; // 通道优先级最高
	DMA1_Channel1->CCR &= ~(3<<10); // 存储器数据宽度 8位
	DMA1_Channel1->CCR &= ~(3<<8); // 外设数据宽度 8bit
	
	DMA1_Channel1->CCR |= 1<<7; // 存储器地址增量模式 开启
	DMA1_Channel1->CCR |= 1<<6; // 外设地址增量模式 开启
	DMA1_Channel1->CCR |= (1<<5); // 开启循环模式
	
	DMA1_Channel1->CCR |= 1<<4; // 数据传输方向，从存储器读
	
  DMA1_Channel1->CCR |= 1<<0; // 开启传输
}

/**
   如果初始化 不执行单次循环，需要手动进行一次数据传输
*/ 
void start_Transt() 
{
  DMA1_Channel1->CCR &= ~(1<<0); // 禁止传输
  DMA1_Channel1->CNDTR = DMA1_MEM_LEN;
	DMA1_Channel1->CCR |= 1<<0; // 开启传输
}
