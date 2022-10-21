#include "reg.h"
#include "beep.h"

void beep_init(void) {
	
	// 1、使能APB2
  // 2  设置端口的输出模式
  GPIOB_CRH &= ~(3<<2*1); //通用推挽输出方式
  // 3 设置端口的速率  
	GPIOB_CRH |= 3<<0;//端口频率设置为2MHZ
	
	BEEP = 0;

	
}
