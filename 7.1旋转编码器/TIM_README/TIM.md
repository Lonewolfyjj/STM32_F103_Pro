TIM3 定时器中断笔记



1.设置时钟源,采用外部高速时钟 F103是最大72MHz

<img src="TIM_SYSCLK.png" alt="`TIM_SYSCLK`" style="zoom:38%;" />

2.APB1总线 挂载的TIM3的时钟

3.TIM3涉及到的寄存器

1. CR1 

   ​    [0] 使能计数器

​           [4] DIR 设置计数方向

​           [6:5] CMS 对齐模式

​           [7] ARPE：自动重装载预装载允许位

​         `TIM3->CR1 |= 0X0081;` 

2. 设置预分频器(TIM3_PSC)  APB1 36Mhz 经过自动倍频后给TIM3，分频7200，可以得到10Khz的频率（0.0001s）

   `TIM3->PSC = 7200-1;`

3. 设置自动重装载寄存器(TIM3_ARR) 自动装入初值

   `TIM3->ARR = 5000-1;` //5000次触发一次计数，则输出的定时时间为0.5s

4. 设置中断使能寄存器(TIM3_DIER)  开启中断

5. 设置中断优先级，和中断回调  

工作原理：

<img src="TIMER.png" alt="`TIM_SYSCLK`" style="zoom:38%;" />



**TIMER3 的PWM 功能**

1. 查阅中文手册介绍，涉及如下寄存器,学习每一个对应的寄存器的配置。

<img src="TIMER_PWM.png" alt="`TIM_SYSCLK`" style="zoom:38%;" />

2. 工作原理

脉冲宽度调制模式可以产生一个由TIMx_ARR寄存器确定频率、由TIMx_CCRx寄存器确定占空
比的信号。  

<img src="PWM.png" alt="`TIM_SYSCLK`" style="zoom:38%;" />

通道1PWM 产生电路图

捕获比较寄存器(CCMR1)比较当前计数值（CNT）和装载CCR1的值，如果大于则溢出产生一个脉冲翻转信号.循环往复，就产生了PWM

通过配置对应寄存器使STM32在外部8Mhz 的时钟下对通道2 （LED PB5引脚）输出PWM



3. 让对应引脚进行复用。查阅手册

<img src="PWM_AFIO_PB5.png" alt="`TIM_SYSCLK`" style="zoom:45%;" />

将此引脚配置成TIMER3 通道2的引脚输出。开发板直接连接LED0.可以观察PWM效果

```c
RCC->APB1ENR|=1<<1; 	//TIM3时钟使能    
RCC->APB2ENR|=1<<3;    	//使能PORTB时钟	
GPIOB->CRL&=0XFF0FFFFF;	//PB5输出
GPIOB->CRL|=0X00B00000;	//复用功能输出 	  	 
   
RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
AFIO->MAPR&=0XFFFFF3FF; //清除MAPR的[11:10]
AFIO->MAPR|=1<<11;      //部分重映像,TIM3_CH2->PB5
```



4. 寄存器配置

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

   

功能拓展：

​	将PWM 和 定时器结合，定时器产生中断，来修改CCR2的比较值，就可以实现呼吸灯效果。

参考github