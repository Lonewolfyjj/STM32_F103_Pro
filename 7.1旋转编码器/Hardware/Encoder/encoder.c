#include "encoder.h"
#include "led.h"

void Init_Encoder()
{
  // GPIO输入引脚初始化
	
	Init_GPIO();
	// 定时器初始化
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
	// 使能TIM2挂载的总线
	RCC->APB1ENR |= 1<<0;
	
	/*时基单元配置*/
	TIM2->CR1 |= 0X0081;  // 使能定制器，并设置计数器向上计数，边沿对齐模式
	TIM2->ARR = 65536 - 1; // 满量程计数
  TIM2->PSC = 1 - 1; //  不分頻，編碼器时钟直接驱动定时器

	/*编码器接口配置*/
	TIM2->SMCR |= 0x3; // 工作在从模式的编码器模式3
	TIM2->CCMR1 |= 0x01;// IC1FP1连接在通道1
	TIM2->CCMR2 |= 0x01;// IC2FP2连接在通道2
	
  // CC1输入通道使能，输入不反向；CC2 输入通道使能，输入不反向
	TIM2->CCER &= 0xFF00;
	TIM2->CCER |= 0x0011;
	
	TIM2->CR1 |= 0x01; // 使能计数器
	
  //TIM2->DIER |= 1<<0; // 设置中断使能
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
//  // 初始化中断表
//  // 如果屏蔽EXIT4中断。则在ICER[channel/32] |= 1<<(channel%32) 寄存器中写入对应位1;
//	// 在ARIC 中设置抢占优先级和响应优先级 
//	temp = SCB->AIRCR; // 读取先前的值
//	temp &= 0X0000F8FF;  // 清空高位，并给BIT[10：8] 置0
//  temp |= (0X05FA<<16 | (7 - group) << 8);  // 设置秘钥并设置优先级设置分组3
//	SCB->AIRCR = temp;
//	
//  // 5.配置中断分组(nvic) 并使能中断
//	// NVIC_ISERx  channel = 9 代表外部中断4
//	NVIC->ISER[interrupt_channel/32] |= (1<<(interrupt_channel%32)); // 使能EXIT4 中断位
//  NVIC->IP[interrupt_channel] = priority << 4;  //IPn[7:4] 表示优先级： 当前中断优先级0111  表示 抢占3，响应1
//}
