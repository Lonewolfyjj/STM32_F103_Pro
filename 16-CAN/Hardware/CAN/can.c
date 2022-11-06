#include "can.h"
#include "usart.h"
#include "delay.h"

CanTxMsg TxMessage;
uint8_t flag;
CanRxMsg RxMsg;

void Init_CAN()
{
	CAN_Gpio_Config();
  CAN_Mode_Config();
	CAN_Fileter_Config();
  CAN_Config_TxMessage();
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
  NVIC_Config();	
}
void CAN_Gpio_Config() 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* CAN1 Periph clocks enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap2_CAN1 , ENABLE);
	
	// PA12 -- CAN_TX  PA11 -- CAN_RX 
	// 与串口共用引脚，CAN1_RX重映射--PD0 
  /* Configure CAN1 RX pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
  /* Configure CAN1 TX pin 
     CAN1_tX重映射--PD1 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

}

void CAN_Mode_Config() 
{
	CAN_InitTypeDef CAN_InitStructure; // CAN_StructInit(&CAN_InitStructure);
  /* CAN1 register init */
  CAN_DeInit(CAN1);
	
  /* CAN1 cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE; // 加入时间戳
  CAN_InitStructure.CAN_ABOM = ENABLE;  // 使能自动离线管理
  CAN_InitStructure.CAN_AWUM = DISABLE; // 自动唤醒模式
  CAN_InitStructure.CAN_NART = DISABLE; // 错误重传
  CAN_InitStructure.CAN_RFLM = DISABLE; // 是否锁定FIFO
  CAN_InitStructure.CAN_TXFP = ENABLE; // 按照默认优先级发送
  CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack; // 回环模式，调试成功后设置为正常模式
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;  
  CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
  CAN_InitStructure.CAN_Prescaler = 4; 
	
  /*Initializes the CAN1*/
  CAN_Init(CAN1, &CAN_InitStructure);

}

void CAN_Fileter_Config()
{
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
  /* CAN1 filter init */
  CAN_FilterInitStructure.CAN_FilterNumber = 1; // 设置筛选器1 
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = ((STD_Fileter_ID << 3 | CAN_ID_STD | CAN_RTR_DATA) & 0xFFFF0000)>>16;
  CAN_FilterInitStructure.CAN_FilterIdLow = ((STD_Fileter_ID << 3 | CAN_ID_STD | CAN_RTR_DATA) & 0xFFFF); 
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0; // 设置筛选在FIFO 0 中
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

}
 
void CAN_Config_TxMessage() 
{
  TxMessage.StdId = 0x123;
  TxMessage.ExtId = 0;
  TxMessage.RTR = CAN_RTR_DATA; // 数据帧
  TxMessage.IDE = CAN_ID_STD; // 标准帧
  TxMessage.DLC = 1;  // 数据位1位
}


void CAN_SendMessage(uint8_t data) 
{
 /* Transmit */
	TxMessage.Data[0] = data;
  CAN_Transmit(CAN1, &TxMessage);
}

void NVIC_Config()
{
  NVIC_InitTypeDef  NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

void USB_LP_CAN1_RX0_IRQHandler() 
{
  CAN_Receive(CAN1,CAN_FIFO0,&RxMsg); 
  delay_ms(1);
	printf("\r\nRecv data = 0x%x %x %x %x %x %x %x\r\n",RxMsg.StdId,RxMsg.ExtId,RxMsg.IDE,RxMsg.RTR,RxMsg.DLC,RxMsg.Data[0],RxMsg.FMI);

  flag = 1;

}

