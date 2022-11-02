#include "reg.h"
#include "delay.h"
#include "typedef.h"
#include "usart.h"
#include "timer.h"
#include "spi.h"
#include "led.h"
#include "w25q128.h"

void Rcc_init(void);
u8 readBuff[4096];
u8 writeBuff[40]="hello world this is flash test msg!";// 最多写256字节，文档要求，再多的需要再次发起写命令
int main()
{ 
	u16 i;

	Rcc_init();
	delay_init();
	led_init();
  usart_init(72,115200);

	W25Q128_Init();
  W25Q128_Sector_Erase(0);		//擦除这个扇区
  W25Q128_Write(writeBuff,3,256);
  W25Q128_Read(readBuff,0,4096);
	
  for(i=0;i<4096;i++) {
	  printf(" 0x%x ",readBuff[i]);
		if(i%10==0) printf("\r\n");
	}
	
	
	printf("type = 0x%x\r\n", Read_Manufacturer());
	while(1)
	{ 
    LED_RED=0;
		delay_ms(500);
	  LED_RED=1;
		delay_ms(500);
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
	RCC1->CFGR &= ~(1<<7); // AHB=DIV1;   
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


