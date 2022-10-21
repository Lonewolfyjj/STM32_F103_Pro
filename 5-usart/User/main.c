#include "reg.h"
#include "key.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "typedef.h"
#include "string.h"
#include "usart.h"

void Rcc_init(void);
void key_test(void);

int main()
{
	unsigned short t; 
	unsigned short len;
	unsigned short times=0;
	Rcc_init();
	delay_init();
	led_init();
	key_init();
	beep_init();
	usart_init(72,115200);
	
	while(1)							
	{
    
	 if(USART_RECVICE_STA==2)
	  {					   
			len = R_LEN;//得到此次接收到的长度
			printf("\r\n你发的数据为:\r\n\r\n");
			for(t=0;t<len;t++)
			{
				USART_DR=USART_RX_BUFFER[t];
				while((USART_SR&0X40)==0);//等待发送结束
			}
			printf("\r\n\r\n");//插入换行
			USART_RECVICE_STA=0; // 接收状态归位
			R_LEN = 0; // 长度置0
			memset(USART_RX_BUFFER, 0x00, sizeof(USART_RX_BUFFER)); // 数组清空
		}
		else
		{
			times++;
			if(times%5000==0)
			{
				usart_send("\r\nSTM32F103开发板\r\n");
			} 
			if(times%200==0)
			{
				usart_send("输入数据以回车键结束\r\n");  
				LED_RED = ~LED_RED;
				LED_BLUE = ~LED_BLUE;
			}
			delay_ms(10); 		
		}	
	}
}


void key_test(void) 
{
	unsigned short keyres;
	keyres = key_scan();
	if (keyres == KEY0_PRES) {
		GPIOB_ODR |= 1<<5; 
		GPIOE_ODR &= ~(1<<5);
		BEEP=1;
	} 
	if (keyres == KEY1_PRES){
		GPIOB_ODR &= ~(1<<5);
		GPIOE_ODR |= 1<<5; 
		BEEP=0;
	}
}

void Rcc_init(void) 
{
	// 1.RCC 寄存器打开内部时钟
	RCC1->CR |= 1<<0;
	// 2. 关闭RCC_CIR 中断，避免中断干扰时钟设置
  RCC1->CIR = 0;
  // 3. 设置HSE 的信号源外部晶振不旁路，启动HSE 	
	RCC1->CR &= ~(1<<18);
	// 4. 开启HSE
	RCC1->CR |= 1<<16;
	// 5. 等待HSE 就绪
	while((RCC->CR & (1<<17)) == 0);
	// 6. 配置 AHP APB1 APB2分频倍频系数
	RCC1->CFGR &= ~(1<<4); // AHB=DIV1;
	RCC1->CFGR |= 4<<8;    // APB1=DIV2
	RCC1->CFGR &= ~(1<<13); // APB2=DIV1;
	// 7. 关闭PLL
	RCC1->CR &= ~(1<<24);  // 关闭PLL
	// 8. 配置PLL 分频倍频
	RCC1->CFGR |= 0xf<<18; // 16 倍频
	RCC1->CFGR |= 1<<16; // PLLSRC时钟源选择
	RCC1->CFGR |= 1<<17; //PLLXTPRE 分频选择
  FLASH_ACR |=0x32; //FLASH 2个延时周期
  // 9. 使能PLL
	RCC1->CR |= (1<<24); // 开启PLL
	// 10. 等待PLL 使能OK
	while((RCC1->CR & (1<<25)) == 0); 
	// 11. 选择主PLL 作为时钟
	RCC1->CFGR &= ~(3<<0); // 清零
	RCC1->CFGR |= 2<<0;
	// 12.等待主PLL稳定
	while((RCC1->CFGR & (3<<2)) != (2<<2));	

}


