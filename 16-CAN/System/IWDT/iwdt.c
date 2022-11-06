#include "iwdt.h"
#include "sys.h"

void init_iwdt()
{
	
	IWDG->KR |= 0x5555;
	IWDG->PR |= 0x02; // 独立看门狗16分频 
  IWDG->RLR = 0XFFF; // 16分频 装在值是0xfff ,经过1638.4ms 复位 计算方法参考数据手册

  IWDG->KR |= 0xAAAA; // reload
	
	IWDG->KR |= 0xCCCC; // enable iwdg
}

void iwdt_feed() 
{
   IWDG->KR |= 0xAAAA;
}

