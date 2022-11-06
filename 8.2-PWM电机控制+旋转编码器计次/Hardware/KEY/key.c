#include "reg.h"
#include "delay.h"
#include "key.h"
#include "typedef.h"
#include "beep.h"
#include "led.h"
#include "sys.h"
#include "timer.h"

int32_t duty = 0;

void key_init(void) {
		
	unsigned short interrupt_channel4 = EXTI4_IRQn;
	unsigned short interrupt_channel3 = EXTI3_IRQn;
	unsigned char group = 3;
	unsigned char priority = 0x7;
	volatile unsigned int temp;
	// 1.配置对应引脚
	GPIOE_CRL &= ~(0XF << 16);
	GPIOE_CRL |= 0x8 << 16;// 设置PE4 上下拉输入模式 2MHZ
  GPIOE->ODR|=1<<4;	   	//PE4 上拉
	
  GPIOE_CRL &= ~(0XF << 12);
	GPIOE_CRL |= 0x8 << 12;// 设置PE3 上下拉输入模式 2MHZ
  GPIOE->ODR|=1<<3;	   	//PE3 上拉
	
	
	// 2.使能对应总线时钟
	RCC->APB2ENR |= 1<<6; // 使能APB2总线时钟
	RCC->APB2ENR |= 0x01;//使能io复用时钟	
	// 3.设置PE4 IO 与中断之间的映射关系
	AFIO->EXTICR[1] &= 0xfff0; 
	AFIO->EXTICR[1] |= 0xfff4; // PE4 配置为外部中断4
	
  // 设置PE3 IO 与中断之间的映射关系
	AFIO->EXTICR[0] &= 0x0fff; 
	AFIO->EXTICR[0] |= 0x4fff; // PE3 配置为外部中断3
	
	// 4.初始化线上中断，设置触发条件
	EXTI->IMR |= (1<<4 | 1<<3); // 放开来自线3和4上的中断
	EXTI->FTSR |= (1<<4 | 1<<3); // 允许线3和4的下降沿触发中断
	
  // 如果屏蔽EXIT4中断。则在ICER[channel/32] |= 1<<(channel%32) 寄存器中写入对应位1;
	// 在ARIC 中设置抢占优先级和响应优先级 
	temp = SCB->AIRCR; // 读取先前的值
	temp &= 0X0000F8FF;  // 清空高位，并给BIT[10：8] 置0
  temp |= (0X05FA<<16 | (7 - group) << 8);  // 设置秘钥并设置优先级设置分组3
	SCB->AIRCR = temp;
	
  // 5.配置中断分组(nvic) 并使能中断
	// NVIC_ISERx channel = 9 //代表外部中断4
	NVIC->ISER[interrupt_channel4/32] |= (1<<(interrupt_channel4%32)); // 使能EXIT4 中断位
  NVIC->IP[interrupt_channel4] = priority << 4;  //IPn[7:4] 表示优先级： 当前中断优先级0111  表示 抢占3，响应1
	
	
  NVIC->ISER[interrupt_channel3/32] |= (1<<(interrupt_channel3%32)); // 使能EXIT3 中断位
  NVIC->IP[interrupt_channel3] = (priority - 1) << 4;  //IPn[7:4] 表示优先级： 当前中断优先级0110  表示 抢占3，响应0
}

//外部中断4服务程序
void EXTI4_IRQHandler(void)
{
	delay_ms(10);	//消抖
	if(KEY0==0)	 	//按键KEY0
	{
		duty+=5;
		if(duty >= 100)
  	{
			duty = 100;
		}
		setPwmDuty(duty);
	}		 
	EXIT1->PR=1<<4;  //清除LINE4上的中断标志位  
}		

//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
	delay_ms(10);	//消抖
	if(KEY1==0)	 	//按键KEY0
	{
		duty-=5;
		if(duty <=0)
		{
			 duty = 0;
		}
		setPwmDuty(duty);
	}		 
	EXIT1->PR=1<<3;  //清除LINE3上的中断标志位  
}		

unsigned int key_scan(void) 
{
  static unsigned char key_flag = 1; // 按键状态 1表示叹谈起，0表示按下 
	if ((key_flag == 1) && (KEY0 == 0 || KEY1== 0)) {
		delay_ms(10);
		if (KEY0 == 0 || KEY1 == 0) {
			key_flag = 0;
			if (KEY0 == 0) {
				return KEY0_PRES;
			} 
			if (KEY1 == 0) {
				return KEY1_PRES;
			} 
		}
	}
	
	if ((key_flag == 0) && KEY0 == 1 && KEY1== 1) {
		delay_ms(10);
		if(KEY0 == 1 && KEY1== 1) {
		  key_flag = 1;
		}	
	}

  return NO_KEYPRESS;

}
