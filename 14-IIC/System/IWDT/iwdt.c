#include "iwdt.h"
#include "sys.h"

void init_iwdt()
{
	
	IWDG->KR |= 0x5555;
	IWDG->PR |= 0x02; // �������Ź�16��Ƶ 
  IWDG->RLR = 0XFFF; // 16��Ƶ װ��ֵ��0xfff ,����1638.4ms ��λ ���㷽���ο������ֲ�

  IWDG->KR |= 0xAAAA; // reload
	
	IWDG->KR |= 0xCCCC; // enable iwdg
}

void iwdt_feed() 
{
   IWDG->KR |= 0xAAAA;
}

