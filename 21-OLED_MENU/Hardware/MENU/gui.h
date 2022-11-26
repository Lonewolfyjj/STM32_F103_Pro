#ifndef _GUI_H_
#define _GUI_H_


#include "stm32f10x.h"

#define ON 1
#define OFF 0

typedef struct
{
	u8 Cur_Index;//��ǰ������
	u8 previous;//��һҳ
	u8 next;//��һҳ
	u8 enter;//ȷ��
	u8 back;//����
	void (*current_operation)(u8,u8);//	��ǰ����ִ�еĺ���(����)
}Main_Menu;

//�����������ֵ
enum
{
	_Main_UI=0,
	_Wifi_Option,
	_Bluetooth_Option,
	_Setting_Option,
	_Info_Option,
	_Wifi_Child,
	_Bluetooth_Child,
	_Setting_Child,
	_Info_Child,
	_OLED_Lock,
    _Time_Child,
    
};

//��������ֵ
enum
{
	KEY_PREVIOUS=1,
    KEY_NEXT=2,
	KEY_ENTER=3,
	KEY_BACK=4
};

void Main_UI(u8 page_index,u8 key_val);
void Main_Menu_Func(u8 page_index,u8 key_val);
void Wifi_Child(u8 page_index,u8 key_val);
void Bluetooth_Child(u8 page_index,u8 key_val);
void Setting_Child(u8 page_index,u8 key_val);
void Info_Child(u8 page_index,u8 key_val);
void OLED_Lock(u8 page_index,u8 key_val);
void GUI_Refresh(void);
void Time_Child(u8 page_index,u8 key_val) ;
void Beep_App(void);
void LED_RED_App(void);
void LED_GREEN_App(void);

#endif
