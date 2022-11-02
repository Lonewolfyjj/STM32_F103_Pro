#include "reg.h"
#include "delay.h"
#include "typedef.h"
#include "usart.h"
#include "timer.h"
#include "iic.h"
#include "led.h"

void Rcc_init(void);

int main()
{ 
	u8 data = 1;
	u8 i=0;
	Rcc_init();
	delay_init();
	led_init();
  usart_init(72,115200);
  IIC_Init();
	
	LED_RED = 0;
	//delay_ms(1000);
//	for(i=0;i<=255;i++) {
//	    send_write_byte(i,i+1);
//		  printf("write data = %d\r\n",i);
//  		delay_ms(10);
//		if(i==255) {
//		   break;
//		}
//  }
	
	for(i=0;i<=255;i++) {
	  data = send_read_byte(i); 
		printf("read data = %d\r\n",data);
		LED_RED = data & 0x01; // ��led ָʾ���ݶ�ȡ��
		delay_ms(100);
		if(i==255) {
		   break;
		}
  }

	delay_ms(1000);
	while(1)
	{ 
		LED_RED = 1;
		delay_ms(3000);
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


