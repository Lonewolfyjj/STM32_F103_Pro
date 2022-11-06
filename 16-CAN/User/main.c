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
  printf("CAN 实验，按下KEY0 发送一个数据\r\n");
	while(1)
	{ 
		
		keyScan = key_scan();
		if(keyScan == KEY0_PRES) {
			//for(i=0;i<255;i++) {
		  TxMessage.Data[0] = 22; // 给出发送数据

			mailBox = CAN_Transmit(CAN1,&TxMessage); // 发送数据，返回发送的邮箱
			
      printf(" \r\nsend data = 0x%x %x %x %x %x %x \r\n",TxMessage.StdId,TxMessage.ExtId,TxMessage.IDE,TxMessage.RTR,TxMessage.DLC,TxMessage.Data[0]);
			CAN_TxStatus = CAN_TransmitStatus (CAN1,mailBox); // 检查发送的邮箱发送状态
			printf("CAN TxStatus = %d\r\n",CAN_TxStatus);
			delay_ms(5); // 等待发送完成
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
	// 1.RCC 寄存器打开内部时钟
	RCC->CR |= 1<<0;
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


