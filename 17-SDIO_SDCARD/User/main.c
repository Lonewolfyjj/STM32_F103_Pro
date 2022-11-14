#include "reg.h"
#include "delay.h"
#include "typedef.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "stm32_sdio_sd.h"
#include "key.h"
#include "sd_test.h"

void Rcc_init(void);
uint8_t Buffer_Tx[512] = "ABCDEFGHIGKLMNOPQRSTUVWXYZ", Buffer_Rx[512];
#define BLOCK_SIZE            512 /* Block Size in Bytes */

#define NUMBER_OF_BLOCKS      32  /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * NUMBER_OF_BLOCKS)

extern SD_CardInfo SDCardInfo;
SD_Error Stat = SD_OK;

u8 *Block_Rx;
int main()
{ 
	u16 i;
	Rcc_init();
	delay_init();
	led_init();
  usart_init(72,115200);
  key_init();
	Stat = SD_Init();
	printf("sd init = %d\r\n",Stat);
	printf("sd size = %d MB\r\n",SDCardInfo.CardCapacity >>20); 
  SD_Test();
	
  if(SD_WriteBlock(Buffer_Tx,0,512)==0)	//写入0扇区的内容
	{	
		printf("SECTOR 0 DATA:\r\n");
		for(i=0;i<512;i++) printf("%x ",Buffer_Tx[i]);//打印写入0扇区数据    	   
		printf("\r\nDATA ENDED\r\n");
	}
	
	if(SD_ReadBlock(Buffer_Rx,0,512)==0)	//读取0扇区的内容
	{	
		printf("SECTOR 0 DATA:\r\n");
		for(i=0;i<512;i++) printf("%x ",Buffer_Rx[i]);//打印读取0扇区数据    	   
		printf("\r\nDATA ENDED\r\n");
	}
	
	while(1)
	{ 
		
    LED_RED = 0;
		delay_ms(100);
		LED_RED=1;
		delay_ms(100);
	}
 
}


void Rcc_init(void) 
{
	// 1.RCC 寄存器打开内部时钟
	RCC->CR |= 1<<0;
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


