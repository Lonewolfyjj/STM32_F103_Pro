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
	Rcc_init();
	delay_init();
  beep_init();
	led_init();
	key_init();

	while(1)							
	{
		BEEP=1;
		LED_RED=~LED_RED;
		LED_GRREN=~LED_GRREN;
	  delay_ms(10); 
    BEEP=0;
	  delay_ms(900); 
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
	RCC1->CFGR &= ~(1<<4); // AHB=DIV1;
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


