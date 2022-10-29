#include "adc.h"
#include "sys.h"
#include "delay.h"


void init_adc() 
{
	RCC->APB2ENR |= 1<<9; 	// 配置ADC1 RCC时钟使能
  RCC->APB2ENR |= 1<<2;  // 配置对应通道输入GPIOA引脚的时钟使能
  RCC->CFGR &= ~(3<<14); 
	RCC->CFGR |= 2<<14; // 配置ADC CLK 的时钟频率
	
	RCC->APB2RSTR |= 1<<9; // ADC1接口复位
	RCC->APB2RSTR &= ~(1<<9);//复位结束	 
	
  // 配置对应GPIO 通道引脚的输入模拟模式(adc专属模式)
	GPIOA->CRL &= ~(0XF<<4);
	
	ADC1->CR1 &=  ~(0XF<<16); //设置ADC工作于独立模式
	
	ADC1->CR1 &= ~(1<<8); // 关闭扫描模式	
  ADC1->CR2 |= 1<<1; // 循环转换模式
	
  ADC1->CR2 |= 1<<20;      //使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	ADC1->CR2 |= 7<<17; //选择启动规则通道组转换的外部事件，这里选择软件触发

	ADC1->CR2 &= ~(1<<11); //选择数据右对齐
	
	ADC1->CR2 &= ~(1<8); //不使用DMA模式
	
  ADC1->SMPR2 |= 	7<<3; //ADC 通道1 采样239.5周期确保精度，损失快速
	
	ADC1->SQR1&=~(0XF<<20); //规则通道序列长度定义为1，所以规则通道中只放一个AD数据进行转换。 因为当前我们只采样PA1通道的数据，所以设置为1 ，如果需要多个通道多个转换，需要在加多个规则通道。相当于每个萝卜要在这个坑里长大，ADC 的数据转换也需要在这个规则通道里进行转换。
  
	ADC1->CR2 |= 1<<0; // 开启ADC并启动转换
	
  ADC1->CR2 |= 1<<3; // 开始复位校准
	while(ADC1->CR2&1<<3); // 复位校准完成
	
	ADC1->CR2 |= 1<<2; // 开始校准
	while(ADC1->CR2 & 1<<2); // 校准完成
	
  //设置转换序列	  		 
	ADC1->SQR3 &= 0XFFFFFFE0; // 规则序列1 通道清空
	ADC1->SQR3 |= ADC_CH1; 			// 通道1(连接在PA1)	; // PA1对应通道1		  
  
  ADC1->CR2 |= 1<<22; // ADC开始转换规则通道

} 

uint16_t getADC() 
{

	while(!(ADC1->DR & 1<<1));
  // 采用1个规则组，循环转换模式，直接读取数据就行啦。 
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
	 
