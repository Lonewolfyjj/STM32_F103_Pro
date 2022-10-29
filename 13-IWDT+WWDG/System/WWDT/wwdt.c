#include "wwdt.h"
#include "sys.h"


// 上窗口超时时间(us) = 4096*预分频*（计数器初始值 - 窗口值）/ APB1时钟频率
// 下窗口超时时间(us) = 4096*预分频*（计数器初始值 - 0x40）/ APB1时钟频率
void init_wwdt() 
{
	RCC->APB1ENR|=1<<11; 	//使能wwdg时钟 
  WWDG->CR |= 1<<7; // 使能独立看门狗
	WWDG->CR |= 0x7F; // 存储看门狗的计数器值 0xFF和0xC0  ;
	
	WWDG->CFR |= 0x64; // 100 窗口值
	WWDG->CFR |= 3<<7; // 8分频 //58.25ms 窗口时间

}


void wwdt_feed() 
{
  WWDG->CR |= 0x7F; // 存储看门狗的计数器值 0xFF和0xC0  ;
}
