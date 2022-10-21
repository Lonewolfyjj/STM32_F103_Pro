#include "reg.h"
#include "delay.h"
#include "key.h"

void key_init(void) {
	
	//	GPIOE->CRL&=0XFFF00FFF;	//PE3/4设置成输入	  
	//  GPIOE->CRL|=0X00088000; 				   
	GPIOE->ODR|=3<<3;	   	//PE3/4 上拉
	
  // 设置PE3引脚为输入状态
	GPIOE_CRL &= ~(3<<2*6);  // 设置端口           00111111111111
	GPIOE_CRL &= ~(3<<2*7); // PE3 寄存器清零      10 00000000000000
	GPIOE_CRL |= (2<<2*7);  // 设置PE3寄存器为上/下拉输入模式
	
	// 设置PE4引脚为输入状态
  GPIOE_CRL &= ~(3<<2*8);  // 设置端口           001111111111111111          
	GPIOE_CRL &= ~(3<<2*9); // PE3 寄存器清零      10000000000000000000
	GPIOE_CRL |= (2<<2*9);  // 设置PE3寄存器为上/下拉输入模式
}

unsigned int key_scan(void) 
{
  static unsigned char key_flag = 1; // 按键状态 1表示叹谈起，0表示按下 
	if ((key_flag == 1) && (KEY0 == 0 || KEY1== 0)) {
		delay_ms(10);
		if (KEY0 == 0 || KEY1 == 0) {
			key_flag = 0;
			if (KEY0 == 0) {
				return KEY0_PRES;
			} 
			if (KEY1 == 0) {
				return KEY1_PRES;
			} 
		}
	}
	
	if ((key_flag == 0) && KEY0 == 1 && KEY1== 1) {
		delay_ms(10);
		if(KEY0 == 1 && KEY1== 1) {
		  key_flag = 1;
		}	
	}

  return NO_KEYPRESS;

}
