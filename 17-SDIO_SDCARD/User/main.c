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
	
  if(SD_WriteBlock(Buffer_Tx,0,512)==0)	//д��0����������
	{	
		printf("SECTOR 0 DATA:\r\n");
		for(i=0;i<512;i++) printf("%x ",Buffer_Tx[i]);//��ӡд��0��������    	   
		printf("\r\nDATA ENDED\r\n");
	}
	
	if(SD_ReadBlock(Buffer_Rx,0,512)==0)	//��ȡ0����������
	{	
		printf("SECTOR 0 DATA:\r\n");
		for(i=0;i<512;i++) printf("%x ",Buffer_Rx[i]);//��ӡ��ȡ0��������    	   
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
	// 1.RCC �Ĵ������ڲ�ʱ��
	RCC->CR |= 1<<0;
	// 2. �ر�RCC_CIR �жϣ������жϸ���ʱ������
  RCC1->CIR = 0;
  // 3. ����HSE ���ź�Դ�ⲿ������·������HSE 	
	RCC1->CR &= ~(1<<18);
	// 4. ����HSE
	RCC1->CR |= 1<<16;
	// 5. �ȴ�HSE ����
	while((RCC->CR & (1<<17)) == 0);
	// 6. ���� AHP APB1 APB2��Ƶ��Ƶϵ��
	RCC1->CFGR &= ~(1<<7); // AHB=DIV1;   
	RCC1->CFGR |= 4<<8;    // APB1=DIV2 
	RCC1->CFGR &= ~(1<<13); // APB2=DIV1;
	// 7. �ر�PLL
	RCC1->CR &= ~(1<<24);  // �ر�PLL
	// 8. ����PLL ��Ƶ��Ƶ
	RCC1->CFGR |= 0xf<<18; // 16 ��Ƶ
	RCC1->CFGR |= 1<<16; // PLLSRCʱ��Դѡ��
	RCC1->CFGR |= 1<<17; //PLLXTPRE ��Ƶѡ��
  FLASH_ACR |=0x32; //FLASH 2����ʱ����
  // 9. ʹ��PLL
	RCC1->CR |= (1<<24); // ����PLL
	// 10. �ȴ�PLL ʹ��OK
	while((RCC1->CR & (1<<25)) == 0); 
	// 11. ѡ����PLL ��Ϊʱ��
	RCC1->CFGR &= ~(3<<0); // ����
	RCC1->CFGR |= 2<<0;
	// 12.�ȴ���PLL�ȶ�
	while((RCC1->CFGR & (3<<2)) != (2<<2));	

}


