#include "reg.h"
#include "key.h"
#include "delay.h"
#include "led.h"
void delay(void);
void led_init(void);
void Rcc_init(void);
int main()
{
	unsigned short keyres;
	
	delay_init();
	LED_Init();
	key_init();
	Rcc_init();
	while(1)							
	{
		keyres = key_scan();
		if(keyres == KEY0_PRES) {
			GPIOB_ODR |= 1<<5; 
			GPIOE_ODR &= ~(1<<5);
		} 
		if (keyres == KEY1_PRES){
			GPIOB_ODR &= ~(1<<5);
			GPIOE_ODR |= 1<<5; 
		}
	}
}

void Rcc_init(void) 
{
	// 1.RCC 寄存器打开内部时钟
	RCC_CR |= 1<<0;
	// 2. 关闭RCC_CIR 中断，避免中断干扰时钟设置
  RCC_CIR = 0;
  // 3. 设置HSE 的信号源外部晶振不旁路，启动HSE 	
	RCC_CR &= ~(1<<18);
	// 4. 开启HSE
	RCC_CR |= 1<<16;
	// 5. 等待HSE 就绪
	while((RCC_CR & (1<<17)) == 0);
	// 6. 配置 AHP APB1 APB2分频倍频系数
	RCC_CFGR &= ~(1<<4); // AHB=DIV1;
	RCC_CFGR |= 4<<8;    // APB1=DIV2
	RCC_CFGR &= ~(1<<13); // APB2=DIV1;
	// 7. 关闭PLL
	RCC_CR &= ~(1<<24);  // 关闭PLL
	// 8. 配置PLL 分频倍频
	RCC_CFGR |= 0xf<<18; // 16 倍频
	RCC_CFGR |= 1<<16; // PLLSRC时钟源选择
	RCC_CFGR |= 1<<17; //PLLXTPRE 分频选择
  FLASH_ACR |=0x32; //FLASH 2个延时周期
  // 9. 使能PLL
	RCC_CR |= (1<<24); // 开启PLL
	// 10. 等待PLL 使能OK
	while((RCC_CR & (1<<25)) == 0); 
	// 11. 选择主PLL 作为时钟
	RCC_CFGR &= ~(3<<0); // 清零
	RCC_CFGR |= 2<<0;
	// 12.等待主PLL稳定
	while((RCC_CFGR & (3<<2)) != (2<<2));	
	
	RCC_APB2ENR |= 1<<3; // 使能PB 模块的时钟				
	RCC_APB2ENR |= 1<<6; // 使能PE 模块的时钟		

}

