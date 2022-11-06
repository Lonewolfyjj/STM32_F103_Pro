#include "reg.h"
#include "delay.h"
#include "typedef.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "can.h"
#include "key.h"


void Rcc_init(void);


int main()
{ 
	uint8_t mailBox,CAN_TxStatus,textDat=0,i;
	u8 keyScan;
	Rcc_init();
	delay_init();
	led_init();
  usart_init(72,115200);
  key_init();
	Init_CAN();
  printf("CAN ʵ�飬����KEY0 ����һ������\r\n");
	while(1)
	{ 
		
		keyScan = key_scan();
		if(keyScan == KEY0_PRES) {
			//for(i=0;i<255;i++) {
		  TxMessage.Data[0] = 22; // ������������

			mailBox = CAN_Transmit(CAN1,&TxMessage); // �������ݣ����ط��͵�����
			
      printf(" \r\nsend data = 0x%x %x %x %x %x %x \r\n",TxMessage.StdId,TxMessage.ExtId,TxMessage.IDE,TxMessage.RTR,TxMessage.DLC,TxMessage.Data[0]);
			CAN_TxStatus = CAN_TransmitStatus (CAN1,mailBox); // ��鷢�͵����䷢��״̬
			printf("CAN TxStatus = %d\r\n",CAN_TxStatus);
			delay_ms(5); // �ȴ��������
			if(CAN_TxStatus == CAN_TxStatus_Ok) {
		    LED_RED = 0;
		    printf("CAN msg send success !\r\n");
		  } else if(CAN_TxStatus == CAN_TxStatus_Failed){
			  printf("CAN msg send failed !\r\n");
			} else if(CAN_TxStatus == CAN_TxStatus_Pending){
			  printf("CAN msg pedning !\r\n");
			} else {
			   printf("NO CAN send mailbox!\r\n");
			}
		//}
	}
    
		if(flag) 
    {
		  //printf("recv data = 0x%x %x %x %x  \r\n",CAN1->RI[0]);
			//long *p = ((volatile unsigned long *) ());
//      unsigned long ReadValue = *p;
//      printf(">>>ReadValue :0x%X\n", ReadValue );
//      printf(">>>OSCFF_CTRL:0x%X\n",  (*(volatile unsigned long *)0x4000640C));
	   
         //printf("recv from can1 recv 0x%x\r\n",CAN1->sFIFOMailBox[0]);
 
			LED_GRREN = 0;
			delay_ms(10);
			LED_GRREN = 1;
			LED_RED = 1;
			flag = 0;
		} 
	}
}


void Rcc_init(void) 
{
	// 1.RCC �Ĵ������ڲ�ʱ��
	RCC->CR |= 1<<0;
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


