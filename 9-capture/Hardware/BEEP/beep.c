#include "reg.h"
#include "beep.h"
#include "typedef.h"
void beep_init(void) {
	
	// 1��ʹ��APB2
	RCC1->APB2ENR = 1<<3;
  // 2  ���ö˿ڵ����ģʽ
  GPIOB_CRH &= ~(3<<2*1); //ͨ�����������ʽ
  // 3 ���ö˿ڵ�����  
	GPIOB_CRH |= 3<<0;//�˿�Ƶ������Ϊ2MHZ
	
	BEEP = 0;

	
}
