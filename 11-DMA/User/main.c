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
	// 1.RCC �Ĵ������ڲ�ʱ��
	RCC1->CR |= 1<<0;
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


