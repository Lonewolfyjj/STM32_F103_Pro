#include "sys.h"
#include "timer.h"
#include "beep.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
uint8_t duty_width;
void init_timer3(u16 psc, u16 arr) 
{

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

void init_timer3_pwd() 
{	
  //此部分需手动修改IO口设置
	RCC->APB1ENR|=1<<1; 	//TIM3时钟使能    
	RCC->APB2ENR|=1<<3;    	//使能PORTB时钟	
	GPIOB->CRL&=0XFF0FFFFF;	//PB5输出
	GPIOB->CRL|=0X00B00000;	//复用功能输出 	  	 
	   
	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
	AFIO->MAPR&=0XFFFFF3FF; //清除MAPR的[11:10]
	AFIO->MAPR|=1<<11;      //部分重映像,TIM3_CH2->PB5
	
	TIM3->EGR |= 1<<0; //初始化所有的寄存器
	
  TIM3->PSC = 7200-1;  // 得到 10khz的分频
	TIM3->ARR = 100;     // 每次到100HZ.重新进行装载 ：10KZH --> 100HZ
	
  //在TIMx_CCMRx寄存器中的OCxM位写入’110’(PWM模式1)或’111’(PWM模式2)，能够独立地设置每个OCx输出通道产生一路PWM
	TIM3->CCMR1 |= 6<<12; // 通道2 PWM模式，输出比较1模式。
	TIM3->CCMR1 |= 1<<11;  // 输出比较2预装载使能 
	TIM3->CCMR1 |= 1<<3;  // 开启TIMx_CCR1寄存器的预装载功能(寄存器) 读写操作仅对预装载寄存器操作， TIMx_CCR1的预装载值在更新事件到来时被加载至当前寄存器中。
  TIM3->CCMR1 &= ~(2<<0); //CC1通道被配置为输出	
	
	// 使能定制器，并设置计数器向上计数，边沿对齐模式
	TIM3->CR1 |= 0X0081; 	
	TIM3->CCER |= 1<<4; //OC2 输出使能
	TIM3->CCER &= ~(1<<5);// OC2高电平有效

  // 修改这个值5000，配合AAR寄存器与装载值10000，产生1hz的 50%占空比PWM
  TIM3->CCR2 = 50; // PB5链接通道2所以是CCR2 : 50% 1HZ PWM
	
  //TIM3->DIER |= 1<<0; // 设置中断使能
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
		
		TIM3->CCR2 = temp; // 改变CCR2值，可以改变占空比
		TIM3->SR &= ~(1<<0);
	}
	
}	

