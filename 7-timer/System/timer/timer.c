#include "sys.h"
#include "timer.h"
#include "beep.h"
#include "led.h"
#include "delay.h"

void init_timer3(u16 psc, u16 arr) 
{
	// 设置时钟源,采用外部高速时钟
	
	// 使能TIM3挂载的总线
	RCC->APB1ENR |= 1<<1;
	// 使能定制器，并设置计数器向上计数，边沿对齐模式
	TIM3->CR1 |= 0X0081; 
	TIM3->ARR = arr-1; 
	TIM3->PSC = psc-1;  // 得到 10khz的分频
  TIM3->DIER |= 1<<0; // 设置中断使能
	init_my_nvic(TIM3_IRQn);
} 

void init_my_nvic(u8 interrupt_channel) {

	unsigned char group = 4;
	unsigned char priority = 0x7;
	volatile unsigned int temp;
	
  // 初始化中断表
  // 如果屏蔽EXIT4中断。则在ICER[channel/32] |= 1<<(channel%32) 寄存器中写入对应位1;
	// 在ARIC 中设置抢占优先级和响应优先级 
	temp = SCB->AIRCR; // 读取先前的值
	temp &= 0X0000F8FF;  // 清空高位，并给BIT[10：8] 置0
  temp |= (0X05FA<<16 | (7 - group) << 8);  // 设置秘钥并设置优先级设置分组3
	SCB->AIRCR = temp;
	
  // 5.配置中断分组(nvic) 并使能中断
	// NVIC_ISERx  channel = 9 代表外部中断4
	NVIC->ISER[interrupt_channel/32] |= (1<<(interrupt_channel%32)); // 使能EXIT4 中断位
  NVIC->IP[interrupt_channel] = priority << 4;  //IPn[7:4] 表示优先级： 当前中断优先级0111  表示 抢占3，响应1
}

void TIM3_IRQHandler(void)
{
	if(TIM3->SR & 0x0001){
		LED_RED = ~LED_RED;
		TIM3->SR &= ~(1<<0);
	}
	
}	

