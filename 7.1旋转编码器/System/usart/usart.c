#include "usart.h"
#include "reg.h"
#include "sys.h"
#include "typedef.h"	

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定向fputc函数
//printf的输出，指向fputc，由fputc输出到串口
//这里使用串口1(USART1)输出printf信息
int fputc(int ch, FILE *f)
{      
	while((USART_SR&0X40)==0);//等待上一次串口数据发送完成  
	USART_DR = (u8) ch;      	//写DR,串口1将发送数据
	return ch;
}
#endif 

unsigned char USART_RX_BUFFER[USART_REC_LENGTH];
unsigned short USART_RECVICE_STA = 0;
unsigned short R_LEN = 0;

void usart_init(u32 pclk2,u32 bound) 
{
	float temp;
	unsigned short int mantissa;
	unsigned short int fraction;	
  
	// 波特率固定计算，参考25.3.4
	temp = (float)(pclk2*1000000 / (16*bound));
  mantissa = temp; //得到整数部分
	fraction = (temp - mantissa)*16; // 得到小数部分
	mantissa<<=4;
	mantissa+=fraction;
	
  RCC1->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC1->APB2ENR|=1<<14;  //使能串口时钟 
	
	//A IO端口设置
	GPIOA_CRH &= ~(0XFF<<4); // io置位
	GPIOA_CRH |= 0x8b<<4; // PA10 上下拉输入  PA9 复用推挽输出50Mhz ???
	
	RCC1->APB2RSTR |= 1<<14; // 复位串口1
  RCC1->APB2RSTR &= ~(1<<14);	// 停止复位串口1
	
	USART_BRR = mantissa;   // 0-3 小数位  4-15 整数位 
	USART_CR1 |= 1<<13; // 串口1 使能
	USART_CR1 |= 0xc;   // 发送使能接受使能    
	USART_CR1 |= 1<<5;  //接收缓冲区非空中断使能
	
	MY_NVIC_Init(3 , 3, USART1_IRQn, 2);//组2，最低优先级 
}

void usart_send(char *s) {
	while(*s)
	{
		while((USART1->SR&0X40)==0);//等待上一次串口数据发送完成  
    USART_DR = (u8) *s++;      	//写DR,串口1将发送数据
	}
}

void USART1_IRQHandler(void) 
{
	unsigned char res;	

	if(USART_SR & (1<<5))	//接收到数据
	{	 
		res = USART_DR; 
		if(USART_RECVICE_STA != 2) //接收未完成
		{
			if(USART_RECVICE_STA == 1)//接收到了0x0d
			{
				if(res != 0x0a)
					USART_RECVICE_STA = 0; //接收错误,重新开始
				else
					USART_RECVICE_STA = 2;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(res == 0x0d)
					USART_RECVICE_STA = 1;
				else
				{
					USART_RX_BUFFER[R_LEN] = res;
					R_LEN++;
					if(R_LEN > (USART_REC_LENGTH - 1))
						USART_RECVICE_STA = 0;  //接收数据错误,重新开始接收	  
				}
			}
		}  		 									     
	}
}
