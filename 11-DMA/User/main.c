#include "reg.h"
#include "key.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "typedef.h"
#include "string.h"
#include "usart.h"
#include "timer.h"
#include "adc.h"
#include "MyDMA.h"


void Rcc_init(void);

int main()
{
  u8 data1[] = {0x01,0x02,0x03,0x04};
  u8 data2[] = {0,0,0,0};
	u16 adcResult;
  Rcc_init();
  delay_init();
  usart_init(72,115200);

  init_adc();
	//DMA_init((uint32_t)&data1,(uint32_t)&data2, sizeof(data2));
	while(1)							
	{
		
		adcResult = Get_Adc_Average();
	  printf("adcResult = %d\r\n",adcResult);
		//LCD_ShowxNum(30,70,adcResult,4,24,0);
//		data1[0]++;
//		data1[1]++;
//		data1[2]++;
//		data1[3]++;
//		LCD_ShowxNum(30,70,(u8)data1[0],3,24,0X80);
//		LCD_ShowxNum(80,70,(u8)data1[1],3,24,0X80);
//		LCD_ShowxNum(130,70,(u8)data1[2],3,24,0X80);
//		LCD_ShowxNum(180,70,(u8)data1[3],3,24,0X80);
//		delay_ms(1000);
//		//start_Transt();
//        LCD_ShowxNum(30,110,(u8)data2[0],3,24,0X80);
//		LCD_ShowxNum(80,110,(u8)data2[1],3,24,0X80);
//		LCD_ShowxNum(130,110,(u8)data2[2],3,24,0X80);
//		LCD_ShowxNum(180,110,(u8)data2[3],3,24,0X80);
//		delay_ms(1000);
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


