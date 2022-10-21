#include "reg.h"
#include "delay.h"
#include "key.h"

void key_init(void) {
	
	//	GPIOE->CRL&=0XFFF00FFF;	//PE3/4���ó�����	  
	//  GPIOE->CRL|=0X00088000; 				   
	GPIOE->ODR|=3<<3;	   	//PE3/4 ����
	
  // ����PE3����Ϊ����״̬
	GPIOE_CRL &= ~(3<<2*6);  // ���ö˿�           00111111111111
	GPIOE_CRL &= ~(3<<2*7); // PE3 �Ĵ�������      10 00000000000000
	GPIOE_CRL |= (2<<2*7);  // ����PE3�Ĵ���Ϊ��/��������ģʽ
	
	// ����PE4����Ϊ����״̬
  GPIOE_CRL &= ~(3<<2*8);  // ���ö˿�           001111111111111111          
	GPIOE_CRL &= ~(3<<2*9); // PE3 �Ĵ�������      10000000000000000000
	GPIOE_CRL |= (2<<2*9);  // ����PE3�Ĵ���Ϊ��/��������ģʽ
}

unsigned int key_scan(void) 
{
  static unsigned char key_flag = 1; // ����״̬ 1��ʾ̸̾��0��ʾ���� 
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
