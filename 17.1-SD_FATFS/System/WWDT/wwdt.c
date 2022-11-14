#include "wwdt.h"
#include "sys.h"


// �ϴ��ڳ�ʱʱ��(us) = 4096*Ԥ��Ƶ*����������ʼֵ - ����ֵ��/ APB1ʱ��Ƶ��
// �´��ڳ�ʱʱ��(us) = 4096*Ԥ��Ƶ*����������ʼֵ - 0x40��/ APB1ʱ��Ƶ��
void init_wwdt() 
{
	RCC->APB1ENR|=1<<11; 	//ʹ��wwdgʱ�� 
  WWDG->CR |= 1<<7; // ʹ�ܶ������Ź�
	WWDG->CR |= 0x7F; // �洢���Ź��ļ�����ֵ 0xFF��0xC0  ;
	
	WWDG->CFR |= 0x64; // 100 ����ֵ
	WWDG->CFR |= 3<<7; // 8��Ƶ //58.25ms ����ʱ��

}


void wwdt_feed() 
{
  WWDG->CR |= 0x7F; // �洢���Ź��ļ�����ֵ 0xFF��0xC0  ;
}
