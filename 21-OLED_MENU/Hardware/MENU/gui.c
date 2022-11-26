#include "gui.h"
#include "bmp.h"
#include "stdio.h"
#include "oled.h"
#include "beep.h"
#include "delay.h"
#include "key.h"
#include "led.h"
#include "rtc.h" 

static u8 func_index=_Main_UI;//��ǰҳ������ֵ
static u8 last_index=_Main_UI;//��һ����������ֵ
static void (*current_operation_func)(u8,u8);//����һ������ָ��
static u8 BEEP_swi=OFF;
static u8 Red_led=OFF;
static u8 Green_led=OFF;

short year = 2000,
        mon = 1 ,
        day = 1,
        hour = 0,
        min = 0,
        sec = 0;


//������
Main_Menu table[20]=
{
	//Cur_Index,previous,next,enter,back,(*current_operation)(u8,u8)
	//������
	{_Main_UI,_Main_UI,_Main_UI,_Wifi_Option,_OLED_Lock,Main_UI},
	//���˵�
	{_Wifi_Option,_Info_Option,_Bluetooth_Option,_Wifi_Child,_Main_UI,Main_Menu_Func},//Wifi
	{_Bluetooth_Option,_Wifi_Option,_Setting_Option,_Bluetooth_Child,_Main_UI,Main_Menu_Func},//����
	{_Setting_Option,_Bluetooth_Option,_Info_Option,_Setting_Child,_Main_UI,Main_Menu_Func},//����
	{_Info_Option,_Setting_Option,_Wifi_Option,_Info_Child,_Main_UI,Main_Menu_Func},//������Ϣ
	
	//�Ӳ˵�
	{_Wifi_Child,_Wifi_Child,_Wifi_Child,_Wifi_Child,_Wifi_Option,Wifi_Child},//Wifi�Ӳ˵�
	{_Bluetooth_Child,_Bluetooth_Child,_Bluetooth_Child,_Bluetooth_Child,_Bluetooth_Option,Bluetooth_Child},//�����Ӳ˵�
	{_Setting_Child,_Setting_Child,_Setting_Child,_Setting_Child,_Setting_Option,Setting_Child},//�����Ӳ˵�
	{_Info_Child,_Info_Child,_Info_Child,_Info_Child,_Info_Option,Info_Child},//������Ϣ�Ӳ˵�
	{_OLED_Lock,_Main_UI,_Main_UI,_Main_UI,_Main_UI,OLED_Lock},//OLEDϨ��
    
    //ʱ������
    {_Time_Child,_Time_Child,_Time_Child,_Time_Child,_Setting_Child,Time_Child},//����ʱ���Ӳ˵�
};


void Main_UI(u8 page_index,u8 key_val)
{
	u8 buff[20];
    OLED_ShowPicture(0,0,11,7, Signal_11x8, 1);//��ʾ�ź���
    OLED_ShowPicture(13,0,9,9,Bluetooth_9x9, 1);//����
    snprintf(buff,sizeof(buff),"%02d-%02d-%02d",calendar.w_year,calendar.w_month,calendar.w_date);
    OLED_ShowString(35,0,buff,8,1); //������
    OLED_ShowPicture(112,0,16,8,BAT_8x16, 1);//���
    snprintf(buff,sizeof(buff),"%02d:%02d",calendar.hour,calendar.min);
    OLED_ShowString(35,12,buff,24,1); //ʱ��
    OLED_ShowChinese(40,48,0,16,1);//��
    OLED_ShowChinese(56,48,1,16,1);//��
    OLED_ShowChinese(72,48,calendar.week + 2,16,1);//��

    OLED_ShowPicture(0,48,16,16,Menu_16x16,1); // �˵�
    OLED_ShowPicture(112,48,16,16,Lock_16x16,1);// ����
}

void Main_Menu_Func(u8 page_index,u8 key_val)
{
	u8 buff[20];
	OLED_ShowPicture(0,0,11,7,Signal_11x8,1);//��ʾ�ź���
	OLED_ShowPicture(14,0,9,9,Bluetooth_9x9,1);//��ʾ����
	snprintf(buff,sizeof(buff),"%02d:%02d:%02d",calendar.hour,calendar.min,calendar.sec);
	OLED_ShowString(35,0,buff,8,1); //ʱ����
	OLED_ShowPicture(112,0,16,8,BAT_8x16, 1);//���
	OLED_ShowPicture(25,24,16,16,Left_16x16,1);//��ʾ���ͷ
	OLED_ShowPicture(90,24,16,16,Right_16x16,1);//��ʾ�Ҽ�ͷ
	OLED_ShowPicture(0,48,16,16,Ok_16x16,1);//��ʾȷ��
	OLED_ShowPicture(112,48,16,16,Back_16x16,1);//��ʾ���ؼ�ͷ
	switch(page_index)
	{
		case _Wifi_Option:
            OLED_ShowPicture(40,12,48,48, Wifi_48x48,1);//��ʾWifiͼ��
			break;
		case _Bluetooth_Option:
            OLED_ShowPicture(48,18,32,32, BT_32x32,1);//��ʾ����ͼ��
			break;
		case _Setting_Option:
            OLED_ShowPicture(48,20,32,32, Setting_32x32,1);//��ʾ����ͼ��
			break;
		case _Info_Option:
            OLED_ShowPicture(48,20,32,32, Info_32x32,1);//��ʾ������Ϣͼ��
			break;
		default:break;
	}
}

void GUI_Refresh(void)
{
	u8 key_val= key_scan();
	if(key_val != NO_KEYPRESS)//ֻ�а������²�ˢ��
	{
		last_index = func_index;//������һ��������ֵ

		switch(key_val)
		{
			case KEY_PREVIOUS :// KEY_PREVIOUS: 
                func_index=table[func_index].previous;//��������ֵ
				break;
			case KEY_ENTER://KEY_ENTER:       
                func_index=table[func_index].enter;//��������ֵ
				break;
			case KEY_NEXT ://KEY_NEXT:
                func_index=table[func_index].next;//��������ֵ
				break;
			case KEY_BACK://KEY_BACK:
                func_index=table[func_index].back;//��������ֵ
				break;
			default:break;
		}
		
		OLED_Clear();//����
        Beep_App();
	}
    
    LED_RED_App();
    LED_GREEN_App();
    
    
    printf("func_index 2 = %d\r\n",func_index);
    
	current_operation_func = table[func_index].current_operation;
	(*current_operation_func)(func_index,key_val);//ִ�е�ǰ������Ӧ�ĺ���
		
}


void Beep_App() 
{
    if(BEEP_swi == ON)
    {
        BEEP=1;
        delay_ms(50);
        BEEP=0;
    }
}


void LED_RED_App() 
{
    if(Red_led == ON)
    {
        LED_RED=1;
        delay_ms(20);
        LED_RED=0;
        delay_ms(20);
    } 
    else 
    {
        LED_RED=1;
    }
}

void LED_GREEN_App()
{
    if(Green_led == ON)
    {
        LED_GRREN=1;
        delay_ms(20);
        LED_GRREN=0;
        delay_ms(20);
    } 
    else 
    {
        LED_GRREN=1;
    }
}


/*
�������ܣ�Wifiѡ���Ӳ˵�
������u8 page_index,u8 key_val
����ֵ����
*/

#define SoftAP_Station 1
#define Station 2 
#define SoftAP 3 
#define REVERSE 0
#define NORMAL 1
void Wifi_Child(u8 page_index,u8 key_val)
{
	static u8 cur_pos=1;
	static u8 wifi_status = ON;
	static u8 wifi_mode = SoftAP_Station;
	static u8 esp_tcpserver = OFF;
	static u8 esp_unvarnishedmode = OFF;
	if(last_index != _Wifi_Option)//�ж��Ƿ��ǵ�һ�ν���˽���
	{
		switch(key_val)
		{
			case KEY_PREVIOUS: 
                cur_pos==1?cur_pos=6:cur_pos--;
				break;
			case KEY_ENTER://ȷ��(����)����
			{
				switch(cur_pos)
				{
					case 1:
                        wifi_status=!wifi_status;
                        if(wifi_status==OFF)
                        {
                            esp_tcpserver=OFF;
                            esp_unvarnishedmode=OFF;
                        }
                        break;
					case 2:
                        if(wifi_status==ON)
                        {
                            wifi_mode==SoftAP_Station?wifi_mode=Station:wifi_mode++;
                            esp_tcpserver = OFF;
                            esp_unvarnishedmode = OFF;
                        }
                        break;
					case 3:
                        if(wifi_status==ON && esp_unvarnishedmode == OFF && (wifi_mode==SoftAP_Station||wifi_mode==SoftAP))
                            esp_tcpserver =! esp_tcpserver;
						break;
					case 4:if(wifi_status==ON&&esp_tcpserver==OFF&&
							(wifi_mode==SoftAP_Station||wifi_mode==Station))esp_unvarnishedmode=!esp_unvarnishedmode;
							break;
					case 5:
							break;
					case 6:
							break;
					default:break;
				}
			}
					break;
			case KEY_NEXT:cur_pos==6?cur_pos=1:cur_pos++;
					break;
			default:break;
		}
	}
	else cur_pos=1;//��һ�ν���˽���,����ָ������
	if(cur_pos<=4)
	{
		OLED_ShowString(0,0,wifi_status ? "1.Switch:ON" : "1.Switch:OFF",16, cur_pos==1 ? REVERSE : NORMAL);
		OLED_ShowString(0,16,wifi_mode == SoftAP_Station ? "2.Mode:AP+STA" : wifi_mode == SoftAP ? "2.Mode:AP" : "2.Mode:STA", 16, cur_pos == 2 ? REVERSE:NORMAL);
		OLED_ShowString(0,32,esp_tcpserver?"3.TCP server:ON":"3.TCP server:OFF",16, cur_pos==3 ? REVERSE : NORMAL);
		OLED_ShowString(0,48,esp_unvarnishedmode ? "4.Mqtt mode:ON" : "4.Mqtt mode:OFF",16,cur_pos == 4 ? REVERSE : NORMAL);
	}
	else if(cur_pos<=8)
	{
		OLED_ShowString(0,0,"5.RSSI:",16, cur_pos==5 ? REVERSE : NORMAL);
		OLED_ShowString(0,16,"6.Esp touch",16, cur_pos==6 ? REVERSE:NORMAL);
	}
}


/*
�������ܣ�����ѡ���Ӳ˵�
������u8 page_index,u8 key_val
����ֵ����
*/
void Bluetooth_Child(u8 page_index,u8 key_val)
{
	static u8 cur_pos=1;
	static u8 BL_status=ON;
	if(last_index!=_Bluetooth_Option)//�ж��Ƿ��ǵ�һ�ν���˽���
	{
		switch(key_val)
		{
			case KEY_PREVIOUS: cur_pos==1?cur_pos=4:cur_pos--;
					break;
			case KEY_ENTER://ȷ��(����)����
			{
				switch(cur_pos)
				{
					case 1:BL_status=!BL_status;
							break;
					case 2:
							break;
					case 3:
							break;
					case 4:
							break;
					default:break;
				}
			}
			break;
			case KEY_NEXT:cur_pos==4?cur_pos=1:cur_pos++;
					break;
			default:break;
		}
	}
	else 
    {
        cur_pos=1;//��һ�ν���˽���,����ָ������
    }
	OLED_ShowString(0,0, BL_status ? "1.Switch:ON":"1.Switch:OFF",16,cur_pos == 1 ? REVERSE : NORMAL);
	OLED_ShowString(0,16, "2.Addr match",16,cur_pos== 2 ? REVERSE : NORMAL);
	OLED_ShowString(0,32, "3.RSSI:",16,cur_pos == 3 ? REVERSE : NORMAL);
	OLED_ShowString(0,48, "4.Mode set",16, cur_pos == 4 ? REVERSE : NORMAL);
}

/*
�������ܣ�����ѡ���Ӳ˵�
������u8 page_index,u8 key_val
����ֵ����
*/
void Setting_Child(u8 page_index,u8 key_val)
{
	char buff[20];
	static u8 cur_pos=1;
	static u8 Remote_form=0;
	static u8 EEPROM_swi=ON;
	static u8 Bri_level=8;
    printf("last_index = %d _Setting_Option =%d \r\n",last_index,_Setting_Option);
	if(last_index!=_Setting_Option)//�ж��Ƿ��ǵ�һ�ν���˽���
	{
		switch(key_val)
		{
			case KEY_PREVIOUS: 
                cur_pos==1?cur_pos=8:cur_pos--;
					break;
			case KEY_ENTER://ȷ��(����)����
			{
				switch(cur_pos)
				{
					case 1:
							break;
					case 2:Remote_form==2 ? Remote_form = 0 : Remote_form++;
							break;
					case 3:Bri_level == 16 ? Bri_level = 1 : Bri_level++;
						   OLED_WR_Byte(0x81,OLED_CMD);//���öԱȶ�
						   OLED_WR_Byte(Bri_level*16-1,OLED_CMD);
							break;
					case 4:BEEP_swi = !BEEP_swi;
							break;
					case 5: Red_led = !Red_led;
							break;
					case 6: Green_led = !Green_led;
							break;
                    case 7: EEPROM_swi = !EEPROM_swi;
                        break;
                    
                    case 8: OLED_Clear();
                        func_index = _Time_Child;                          
                        break;
                            	
					default:break;
				}
			}
					break;
			case KEY_NEXT:cur_pos==8?cur_pos=1:cur_pos++;
					break;
			default:break;
		}
	}
	else cur_pos=1;//��һ�ν���˽���,����ָ������
	
	if(cur_pos<=4)
	{
		OLED_ShowString(0,0, "1.Calib time",16,cur_pos==1 ? REVERSE : NORMAL);
		OLED_ShowString(0,16, Remote_form == 0 ? "2.Remote:NRF2.4G" : Remote_form == 1 ? "2.Remote:Wi-Fi":"2.Remote:BL",16,cur_pos == 2 ? REVERSE : NORMAL);
		snprintf(buff,sizeof(buff),"3.Bri set:%d",Bri_level);//��ʾ����
		OLED_ShowString(0,32,buff,16, cur_pos == 3 ? REVERSE:NORMAL);
		OLED_ShowString(0,48,BEEP_swi ? "4.BEEP:ON":"4.BEEP:OFF",16,cur_pos == 4 ? REVERSE:NORMAL);
	}
	else if(cur_pos<=8)
	{
		OLED_ShowString(0,0, Red_led ? "5.RED LED:OFF":"5.RED LED:ON",16, cur_pos == 5 ? REVERSE : NORMAL);
        OLED_ShowString(0,16, Green_led ? "6.GREEN LED:OFF":"6.GREEN LED:ON",16, cur_pos == 6 ? REVERSE : NORMAL);
        OLED_ShowString(0,32, EEPROM_swi ? "7.EEPROM:ON":"7.EEPROM:OFF",16, cur_pos == 7 ? REVERSE : NORMAL);
		OLED_ShowString(0,48, "8.Time Set",16, cur_pos == 8 ? REVERSE : NORMAL);
	}
}

void Info_Child(u8 page_index,u8 key_val)
{
	char buff[20];
	snprintf(buff,sizeof(buff),"1.BAT vol:%.2f V",2.3f);//��ѹ��Ϊ2
	OLED_ShowString(0,0, buff,16,1);
	OLED_ShowString(0,16,"2.Version:v1.0",16,1);
}
/*
�������ܣ�OLEDϨ��
������u8 page_index,u8 key_val
����ֵ;��
*/
void OLED_Lock(u8 page_index,u8 key_val)
{
	OLED_Clear();
}

void Time_Child(u8 page_index,u8 key_val) 
{   
    static u8 cur_pos=1;
    static u8 first_in=0;
    char buff[20];
    

    if(first_in == 0)
    {
       
        year = calendar.w_year;
        mon = calendar.w_month;
        day = calendar.w_date;
        hour = calendar.hour;
        min = calendar.min;
        sec = calendar.sec;
        first_in = 1; 
    }
   
	if(last_index!=_Setting_Child)//�ж��Ƿ��ǵ�һ�ν���˽���
	{
        
		switch(key_val)
		{
			case KEY_PREVIOUS: 
                if(cur_pos==1) 
                {
                    year++;
                    if(year>2099) {
                        year = 2022;
                    }
                } 
                else if(cur_pos==2) 
                {
                    mon++;
                    if(mon>12) {
                        mon = 1;
                    }
                } 
                else if(cur_pos == 3) 
                {
                    if(((year % 4 == 0) && (year %100 != 0))  || year % 400 == 0) 
                    {  
                        ++day;
                        if(day > 31 && (mon == 1 || mon == 3 || mon == 5 || mon == 7 || mon == 8 || mon == 10 || mon == 12))
                        {
                            day = 1; 
                        } 
                        else if(day > 28 && mon == 2) 
                        {
                            day = 1;
                        } 
                        else if(day > 30) 
                        {
                            day = 1;
                        }
                    } 
                    else 
                    {
                        ++day;
                        if( day > 31 && (mon == 1 || mon == 3 || mon == 5 || mon == 7 || mon == 8 || mon == 10 || mon == 12))
                        {
                            day = 1; 
                        } 
                        else if(day > 28 && mon == 2) 
                        {
                            day = 1;
                        } 
                        else if(day > 30) 
                        {
                            day = 1;
                        }
                    }
                }
                else if (cur_pos==4) 
                {
                    ++hour;
                    if(hour > 23) 
                    {
                        hour = 0;
                    }
                } 
                else if( cur_pos == 5) 
                {
                    ++min;
                    if(min > 59) 
                    {
                        min = 0;
                    }
                }
                else if( cur_pos == 6) 
                {
                    ++sec;
                    if(sec > 59) 
                    {
                        sec = 0;
                    }
                }
                else if(cur_pos == 7 || cur_pos == 8) 
                {
                     RTC_Set((u16)year,(u8)mon,(u8)day,(u8)hour,(u8)min,(u8)sec);
                
                }
                
				break; 
			case KEY_ENTER:// �������
				cur_pos==8 ? cur_pos=1 : cur_pos++;
                break;
			case KEY_NEXT:
                if(cur_pos==1) 
                {
                    if(--year < 2000) {
                        year = 2000;
                    }
                } else 
                if(cur_pos==2) 
                {
                   
                    if(--mon < 1) 
                    {
                        mon = 12;
                    }
                } else 
                if(cur_pos==3) 
                {
                    if(((year % 4 == 0) && (year %100 != 0))  ||year % 400 == 0) 
                    {
                        --day;
                        if(day <1 && (mon == 1 || mon == 3 || mon == 5 || mon == 7 || mon == 8 || mon == 10 || mon == 12))
                        {
                            day = 31; 
                        } 
                        else if(day <1 && mon == 2) 
                        {
                            day = 28;
                        } 
                        else if(day <1) 
                        {
                            day = 30;
                        }
                    } 
                    else 
                    {
                        --day;
                        if( day < 1 && (mon == 1 || mon == 3 || mon == 5 || mon == 7 || mon == 8 || mon == 10 || mon == 12))
                        {
                            day = 31; 
                        } 
                        else if(day <1 && mon == 2) 
                        {
                            day = 29;
                        } 
                        else if(day <1) 
                        {
                            day = 30;
                        }
                    }
                } 
                else if (cur_pos==4) 
                {
                    --hour;
                    if(hour < 0) 
                    {
                        hour = 23;
                    }
                } 
                else if( cur_pos == 5) 
                {
                    --min;
                    if(min < 0) 
                    {
                        min = 59;
                    }
                }
                else if( cur_pos == 6) 
                {
                    --sec;
                    if(sec < 0) 
                    {
                        sec = 59;
                    }
                } 
                else if(cur_pos == 7 || cur_pos == 8) 
                {
                     RTC_Set((u16)year,(u8)mon,(u8)day,(u8)hour,(u8)min,(u8)sec);
                }
              
				break;
             case KEY_BACK:
                 first_in = 0; // �´ν������»�ȡ����ʱ��
                 
                break;
			default:break;
		}
	}
	else 
    {
        cur_pos = 1;//��һ�ν���˽���,����ָ������
    }
    
    snprintf(buff,sizeof(buff),"%02d-%02d-%02d",year,mon,day);
    OLED_ShowString(24,19,buff,16,1); //������
    snprintf(buff,sizeof(buff),"%02d:%02d:%02d",hour,min,sec);
    OLED_ShowString(34,36,buff,16,1); //ʱ����
  
    OLED_ShowNum(24,19,year,4,16, cur_pos == 1 ? REVERSE : NORMAL);
    OLED_ShowNum(64,19,mon,2,16, cur_pos == 2 ? REVERSE : NORMAL);
    OLED_ShowNum(88,19,day,2,16, cur_pos == 3 ? REVERSE : NORMAL);
    
    OLED_ShowNum(34,36,hour,2,16, cur_pos == 4 ? REVERSE : NORMAL);
    OLED_ShowNum(58,36,min,2,16, cur_pos == 5 ? REVERSE : NORMAL);
    OLED_ShowNum(82,36,sec,2,16, cur_pos == 6 ? REVERSE : NORMAL);

    OLED_ShowPicture(0,48,16,16,Ok_16x16, cur_pos == 7 ? REVERSE : NORMAL);//��ʾȷ��
    OLED_ShowPicture(112,48,16,16,Back_16x16,cur_pos == 8 ? REVERSE : NORMAL);//��ʾ���ؼ�ͷ   
}

