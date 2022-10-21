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
			len = R_LEN;//�õ��˴ν��յ��ĳ���
			printf("\r\n�㷢������Ϊ:\r\n\r\n");
			for(t=0;t<len;t++)
			{
				USART_DR=USART_RX_BUFFER[t];
				while((USART_SR&0X40)==0);//�ȴ����ͽ���
			}
			printf("\r\n\r\n");//���뻻��
			USART_RECVICE_STA=0; // ����״̬��λ
			R_LEN = 0; // ������0
			memset(USART_RX_BUFFER, 0x00, sizeof(USART_RX_BUFFER)); // �������
		}
		else
		{
			times++;
			if(times%5000==0)
			{
				usart_send("\r\nSTM32F103������\r\n");
			} 
			if(times%200==0)
			{
				usart_send("���������Իس�������\r\n");  
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


