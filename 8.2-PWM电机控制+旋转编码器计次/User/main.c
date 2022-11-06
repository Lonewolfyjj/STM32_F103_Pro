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
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	
	while(1)
	{
		delay_ms(500);
		speed = getEncoder();
		printf("moto speed = %d/s\r\n",speed*2);
		OLED_Refresh();

		OLED_ShowChinese(0,0,5,16,1);//编
		OLED_ShowChinese(18,0,6,16,1);//码
		OLED_ShowChinese(36,0,7,16,1);//器
		OLED_ShowChinese(54,0,8,16,1);//模
		OLED_ShowChinese(72,0,9,16,1);//式
		OLED_ShowChinese(90,0,10,16,1);//控
		OLED_ShowChinese(108,0,11,16,1);//制
	
		OLED_ShowChinese(0,40,0,16,1);//转
		OLED_ShowChinese(18,40,1,16,1);//速
	
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
		
	  OLED_ShowChinese(0,18,2,16,1);//占
		OLED_ShowChinese(18,18,3,16,1);//空
		OLED_ShowChinese(36,18,4,16,1);//比
		OLED_ShowString(54,18,":100%",16,1);
    OLED_ShowNum(62,18,duty_width,3,16,1);	
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


