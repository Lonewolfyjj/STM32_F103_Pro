#include "sys.h"
#include "timer.h"
#include "beep.h"
#include "led.h"
#include "delay.h"
#include "usart.h"

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

//TIM3 通道1 部分映射CH1/PA6 作为PWM 测量的输入
void timer3_init_input_capture() 
{
  // 使能TIM3的APB1的时钟
  RCC->APB1ENR|=1<<1; 	//TIM3时钟使能    
  RCC->APB2ENR|=1<<2;    	//使能PORT PA6时钟	
  GPIOC->CRL&=0XF0FFFFFF;	//PA6对应位置位
  GPIOC->CRL|=0X08000000;	//PA6 下拉输入 	  	 
  GPIOC->IDR|= 1<<6;      //PA6 输入模式
  
  TIM3->EGR |= 1<<0; //初始化所有的寄存器
  
  TIM3->PSC = 72-1;  // 得到 1mhz的分频
  TIM3->ARR = 100-1;   // 10Khz
  
  TIM3->CCMR1|=1<<0;            //CC1S=01 	选择输入端 IC1映射到TI1上
  TIM3->CCMR1 &= ~(0xf<<4); 		//IC1F=0000 配置输入滤波器 不滤波
  TIM3->CCMR1 &= ~(3<<2);       //IC2PS=00 	配置输入分频,不分频 
  
  TIM3->CCER &= ~(1<<1); 		//CC1P=0	上升沿捕获
  TIM3->CCER |= 1<<0; 		  //CC1E=1 	允许捕获计数器的值到捕获寄存器中
  
  TIM3->SMCR &= ~(7<<4);
  TIM3->SMCR |= 5<<4;     // 滤波后的定时器输入1(TI1FP1)
    
  TIM3->SMCR &= ~(7<<0); 
  TIM3->SMCR |= 4<<0;     // 从模式选择复位模式
  
  TIM3->DIER|= 1<<1;   	//允许捕获中断				
  TIM3->DIER|= 1<<0;   	//允许更新中断	
  TIM3->CR1 |= 0x01;    	//使能定时器3
  
  //通过中断进行判断频率
  init_my_nvic(TIM3_IRQn);
  // 将TIM2输出PWM的通道连接在TIM3 CH1 通道上进行测量
}

//TIM3 通道1 部分映射CH1/PA6 作为PWM 测量的输入
void timer3_init_input_capture_measure_freq() 
{
  // 使能TIM3的APB1的时钟
  RCC->APB1ENR|=1<<1; 	//TIM3时钟使能    
  RCC->APB2ENR|=1<<2;    	//使能PORT PA6时钟	
  GPIOC->CRL&=0XF0FFFFFF;	//PA6对应位置位
  GPIOC->CRL|=0X08000000;	//PA6 下拉输入 	  	 
  GPIOC->IDR|= 1<<6;      //PA6 输入模式
  
  TIM3->EGR |= 1<<0; //初始化所有的寄存器
  
  TIM3->PSC = 72-1;  // 得到 10khz的分频
  TIM3->ARR = 65536-1;  
  
  TIM3->CCMR1|=1<<0;            //CC1S=01 	选择输入端 IC1映射到TI1上
  TIM3->CCMR1 &= ~(0xf<<4); 		//IC1F=0000 配置输入滤波器 不滤波
  TIM3->CCMR1 &= ~(3<<2);       //IC2PS=00 	配置输入分频,不分频 
  
  TIM3->CCER &= ~(1<<1); 		//CC1P=0	上升沿捕获
  TIM3->CCER |= 1<<0; 		  //CC1E=1 	允许捕获计数器的值到捕获寄存器中
  
  TIM3->SMCR &= ~(7<<4);
  TIM3->SMCR |= 5<<4;     // 滤波后的定时器输入1(TI1FP1)
    
  TIM3->SMCR &= ~(7<<0); 
  TIM3->SMCR |= 4<<0;     // 从模式选择复位模式
  
  TIM3->CR1|=0x01;    	//使能定时器3

}

void timer3_init_ic() 
{
	// 使能TIM3的APB1的时钟
	RCC->APB1ENR|=1<<1; 	//TIM3时钟使能    
	RCC->APB2ENR|=1<<2;    	//使能PORT PA6时钟	
	GPIOC->CRL&=0XF0FFFFFF;	//PA6对应位置位
  GPIOC->CRL|=0X08000000;	//PA6 下拉输入 	  	 
  GPIOC->IDR|= 1<<6;      //PA6 输入模式

  TIM3->EGR |= 1<<0; //初始化所有的寄存器
	
  TIM3->PSC = 72-1;  // 得到 1mhz的分频
	TIM3->ARR = 65536-1;    
	
  TIM3->CCMR1|=1<<0;            //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM3->CCMR1 &= ~(0xf<<4); 		//IC1F=0000 配置输入滤波器 不滤波
	TIM3->CCMR1 &= ~(3<<2);       //IC2PS=00 	配置输入分频,不分频 
	
  TIM3->CCMR1 &= ~(3<<8);     
	TIM3->CCMR1 |= 2<<8;       //CC2通道被配置为输入， IC2映射在TI1上
	TIM3->CCMR1 &= ~(3<<10);   //输入捕获2滤波器不滤波;
	TIM3->CCMR1 &= ~(0xf<<12); //输入捕获2分频器不分频;
	
  TIM3->CCER &= ~(1<<1); 		//CC1P=0	上升沿捕获
	TIM3->CCER |= 1<<0; 		  //CC1E=1 	允许捕获计数器的值到捕获寄存器中
	
  TIM3->CCER |= 1<<4;       //CC2E=1 输入/捕获2输出使能
  TIM3->CCER |= 1<<5;       //CC2P=1 下降沿捕获
	
	TIM3->CR2 &= ~(1<<7);   // TIM3 的输入捕获 TI1 连接在CH1 上
	TIM3->CR2 &= ~(3<<4);   // TIM3 输入1 通道产生配置的边沿信号，给从控制一个复位信号，复位CNT计数
	
  TIM3->SMCR &= ~(7<<4);
	TIM3->SMCR |= 5<<4;     // 滤波后的定时器输入1(TI1FP1)
	  
  TIM3->SMCR &= ~(7<<0); 
	TIM3->SMCR |= 4<<0;     // 从模式选择复位模式
	
	
	TIM3->CCER |= 1<<0; // 使能输入捕获1
	TIM3->CCER |= 1<<4; // 使能输入捕获2
	
	TIM3->CR1|=0x01;    	//使能定时器3
}

uint32_t IC_TIM3_CH1_GetDuty() {

	//TIM3->CCR1; 存放周期测量值;
	//TIM3->CCR2 // 存放高电平测量值
	return (TIM3->CCR2 + 1) * 100  / (TIM3->CCR1 + 1);
}

uint32_t IC_TIM3_CH1_GetFreq() 
{
	return 1000000 / (TIM3->CCR1 + 1);
}


// 初始化了TIM2 CH2 通道 PWM1模式, 重映射到CH2/PB3引脚，作为TIM3 PB4引脚的PWM测量输入
// 注意，如果是通过测频法测量频率 这个PMW 发生的频率测量要和捕获的 TIM3->AAR重载值相匹配，不然会频率太慢会导致测量一次超过溢出值，导致测量有问题。
void init_timer2_pwd() {
	
  /*原文链接：https://blog.csdn.net/qq_16963231/article/details/120069444*/
  GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);                 
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //使能SWD 禁用JTAG
  //此部分需手动修改IO口设置
  RCC->APB1ENR|=1<<0; 	//TIM2时钟使能    
  RCC->APB2ENR|=1<<3;    	//使能PORTB时钟	
  GPIOB->CRL&=0XFFFF0FFF;	//PB3输出
  GPIOB->CRL|=0X0000B000;	//复用功能输出 		  	 
  GPIOB->ODR|=1<<3; //PB3配置为输出
  
  RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
  AFIO->MAPR &= ~(3<<8); //清除MAPR的[9:8]
  AFIO->MAPR |= 1<<8;      //部分重映像,TIM2_CH2->PB3
  
  
  TIM2->EGR |= 1<<0; //初始化所有的寄存器
  
  TIM2->PSC = 72-1;  // 得到 10khz的分频
  TIM2->ARR = 1000-1;     // 100ZH 
  
  //在TIMx_CCMRx寄存器中的OCxM位写入’110’(PWM模式1)或’111’(PWM模式2)，能够独立地设置每个OCx输出通道产生一路PWM
  TIM2->CCMR1 |= 6<<12; // 通道2 PWM模式，输出比较1模式。
  TIM2->CCMR1 |= 1<<11;  // 输出比较2预装载使能 
  TIM2->CCMR1 |= 1<<3;  // 开启TIMx_CCR1寄存器的预装载功能(寄存器) 读写操作仅对预装载寄存器操作， TIMx_CCR1的预装载值在更新事件到来时被加载至当前寄存器中。
  TIM2->CCMR1 &= ~(3<<8); //CC2通道被配置为输出	
  
  // 使能定制器，并设置计数器向上计数，边沿对齐模式
  TIM2->CR1 |= 0X0081; 	
  TIM2->CCER |= 1<<4; //OC2 输出使能
  TIM2->CCER &= ~(1<<5);// OC2高电平有效
  
  // 修改这个值5000，配合AAR寄存器与装载值10000，产生1hz的 50%占空比PWM
  TIM2->CCR2 = 400; // PB3链接通道2所以是CCR2 : 50% 1HZ PWM

}


u32 over_flow;
u32 START_VAL,END_VAL;
u16 count,edge;

float measureFrency(void);  // 测量频率
float measureHight(void) ; // 测量高电平时间

void TIM3_IRQHandler(void)
{
		if(TIM3->SR & 0X0001) { // 溢出了一次
			over_flow++;
		}

		if(TIM3->SR & 0x0002){ // 捕获比较寄存器已经捕获到了来自预分频的上升沿，并将计数值存在了CCR1中
			measureHight();
		}

	  TIM3->SR=0;//清除中断标志位 	 
}

float measureHight() 
{
	static float result;
	++edge;
	// 第一个上升沿取值
	if(edge == 1) {
		START_VAL = TIM3->CCR1;
		over_flow=0;
	  TIM3->CCER |= 1<<1; 		//CC1P=0	下降沿捕获
		
	}
	
	// 第一个下降沿取值,一个完整的高电平时间
	if(edge == 2) {
  
		edge = 0;
		END_VAL = TIM3->CCR1;
	  TIM3->CCER &= ~(1<<1); 		//CC1P=0	上升沿捕获
		count = over_flow;
		result = count * 0.001 + START_VAL - END_VAL; // 0.00001是捕获定时器的已知周期
		printf("高电平时间 ====== %f s\r\n",result);
		return result;
	}
	return 0;
}

float measureFrency() {
	static float result;
	++edge;
	if(edge == 1) {
		// 第一个上升沿取值
		START_VAL = TIM3->CCR1;
		over_flow=0;
	} 
	if(edge ==2)
	{
	// 第二个上升沿取值，一个完整的周期
		END_VAL = TIM3->CCR1;
		count = over_flow;
		edge = 0;
	  result = count * 0.001 + END_VAL - START_VAL;
		printf("频率 ====== %f Hz\r\n",result);
		return result;
	}
	return 0;

}

