#include "MyDMA.h"
#include "delay.h"

u8 data1[] = {0x01,0x02,0x03,0x04};
u8 data2[] = {123,123,123,123};

void DMA_init() 
{
	RCC->AHBENR|=1<<0;			//开启DMA1时钟
	delay_ms(5);				//等待DMA时钟稳定

	DMA1_Channel1->CCR |= 1<<14; // 启动存储器到存储器模式
	DMA1_Channel1->CCR |= 3<<12; // 通道优先级最高
	DMA1_Channel1->CCR &= ~(3<<10); // 存储器数据宽度 8位
	DMA1_Channel1->CCR &= ~(3<<8); // 外设数据宽度 8bit
	
	DMA1_Channel1->CCR |= 1<<7; // 存储器地址增量模式 开启
	DMA1_Channel1->CCR |= 1<<6; // 外设地址增量模式 开启
	DMA1_Channel1->CCR |= (1<<5); // 开启循环模式
	
	DMA1_Channel1->CCR |= 1<<4; // 数据传输方向，从存储器读
	
}


void start_Transt(u32 fromAddr,u32 toAddr,u16 count) 
{
	DMA1_Channel1->CMAR = (u32)fromAddr;
		
	DMA1_Channel1->CPAR = (u32)toAddr;
	
  DMA1_Channel1->CNDTR = count; // DMA传输计数器 N 次之后就重装循环
	
	DMA1_Channel1->CCR |= 1<<0; // 开启传输

}
