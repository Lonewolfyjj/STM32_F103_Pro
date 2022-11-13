#ifndef _CAN_H_
#define _CAN_H_
#include "sys.h"

#define EXT_Fileter_ID (uint32_t)(0x1FFF1314)  // EXT模式下扩展帧的位数为[31:3]，不超过限制均可设置
#define STD_Fileter_ID (uint32_t)(0x314)  // STD模式下标准帧STDID[11:0]最多12位，再多不配了

extern uint8_t flag;
extern CanTxMsg TxMessage;
extern CanRxMsg RxMsg;
void Init_CAN(void);
void CAN_Gpio_Config(void) ;
void CAN_Mode_Config(void);
void CAN_Fileter_Config(void);
void CAN_Config_TxMessage(void) ;
void CAN_SendMessage(uint8_t data);
void NVIC_Config(void);
#endif
