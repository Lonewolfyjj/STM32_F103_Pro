#include "reg.h"
#include "key.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "typedef.h"
#include "string.h"
#include "timer.h"
#include "encoder.h"
#include "oled.h"
#include "usart.h"
#include <math.h>

void Rcc_init(void);

int main()
{
	int32_t speed = 0;
	Rcc_init();
	delay_init();
  beep_init();
	led_init();
	key_init();
	usart_init(72,115200);
	init_timer3_pwd();
	setPwmDuty(0);
	Init_Encoder(); 
	OLED_Init();
	OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
  OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
	
	while(1)
	{
		delay_ms(500);
		speed = getEncoder();
		printf("moto speed = %d/s\r\n",speed*2);
		OLED_Refresh();

		OLED_ShowChinese(0,0,5,16,1);//��
		OLED_ShowChinese(18,0,6,16,1);//��
		OLED_ShowChinese(36,0,7,16,1);//��
		OLED_ShowChinese(54,0,8,16,1);//ģ
		OLED_ShowChinese(72,0,9,16,1);//ʽ
		OLED_ShowChinese(90,0,10,16,1);//��
		OLED_ShowChinese(108,0,11,16,1);//��
	
		OLED_ShowChinese(0,40,0,16,1);//ת
		OLED_ShowChinese(18,40,1,16,1);//��
	
	  OLED_ShowString(36,40,":     r/s",16,1);
		if(speed>=0) 
		{
			OLED_ShowChar(44,40,'+',16,1);
		}
		else 
		{
			speed = fabs(speed);
			OLED_ShowChar(44,40,'-',16,1);
		}
		OLED_ShowNum(52,40,speed,4,16,1);   
		
	  OLED_ShowChinese(0,18,2,16,1);//ռ
		OLED_ShowChinese(18,18,3,16,1);//��
		OLED_ShowChinese(36,18,4,16,1);//��
		OLED_ShowString(54,18,":100%",16,1);
    OLED_ShowNum(62,18,duty_width,3,16,1);	
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


