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
	// 1.RCC �Ĵ������ڲ�ʱ��
	RCC_CR |= 1<<0;
	// 2. �ر�RCC_CIR �жϣ������жϸ���ʱ������
  RCC_CIR = 0;
  // 3. ����HSE ���ź�Դ�ⲿ������·������HSE 	
	RCC_CR &= ~(1<<18);
	// 4. ����HSE
	RCC_CR |= 1<<16;
	// 5. �ȴ�HSE ����
	while((RCC_CR & (1<<17)) == 0);
	// 6. ���� AHP APB1 APB2��Ƶ��Ƶϵ��
	RCC_CFGR &= ~(1<<4); // AHB=DIV1;
	RCC_CFGR |= 4<<8;    // APB1=DIV2
	RCC_CFGR &= ~(1<<13); // APB2=DIV1;
	// 7. �ر�PLL
	RCC_CR &= ~(1<<24);  // �ر�PLL
	// 8. ����PLL ��Ƶ��Ƶ
	RCC_CFGR |= 0xf<<18; // 16 ��Ƶ
	RCC_CFGR |= 1<<16; // PLLSRCʱ��Դѡ��
	RCC_CFGR |= 1<<17; //PLLXTPRE ��Ƶѡ��
  FLASH_ACR |=0x32; //FLASH 2����ʱ����
  // 9. ʹ��PLL
	RCC_CR |= (1<<24); // ����PLL
	// 10. �ȴ�PLL ʹ��OK
	while((RCC_CR & (1<<25)) == 0); 
	// 11. ѡ����PLL ��Ϊʱ��
	RCC_CFGR &= ~(3<<0); // ����
	RCC_CFGR |= 2<<0;
	// 12.�ȴ���PLL�ȶ�
	while((RCC_CFGR & (3<<2)) != (2<<2));	
	
	RCC_APB2ENR |= 1<<3; // ʹ��PB ģ���ʱ��				
	RCC_APB2ENR |= 1<<6; // ʹ��PE ģ���ʱ��		

}

