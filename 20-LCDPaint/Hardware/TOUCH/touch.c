#include "touch.h"
#include "ili9341.h"
#include "font.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include "math.h"
#include "stdlib.h"
#include "key.h"
#include "led.h"
#include "palette.h"
#include "w25q128.h"

TP_DEV tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};


// Ĭ�Ϻ���
u8 CMD_RDX = 0X90;
u8 CMD_RDY = 0XD0;

void  XPT2046_Init() 
{
    GPIO_Config();
    tp_dev.init();
    
}

void GPIO_Config() 
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	

  /* ����GPIOʱ�� */
  RCC_APB2PeriphClockCmd ( XPT2046_SPI_GPIO_CLK|XPT2046_PENIRQ_GPIO_CLK, ENABLE );

  /* ģ��SPI GPIO��ʼ�� */          
  GPIO_InitStructure.GPIO_Pin=XPT2046_SPI_CLK_PIN;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz ;	  
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Init(XPT2046_SPI_CLK_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MOSI_PIN;
  GPIO_Init(XPT2046_SPI_MOSI_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MISO_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
  GPIO_Init(XPT2046_SPI_MISO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_CS_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      
  GPIO_Init(XPT2046_SPI_CS_PORT, &GPIO_InitStructure); 
   
  /* ����Ƭѡ��ѡ��XPT2046 */
  XPT2046_CS_LOW();		
								
	//�����������ź�ָʾ���ţ���ʹ���ж�
  GPIO_InitStructure.GPIO_Pin = XPT2046_PENIRQ_GPIO_PIN;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
  GPIO_Init(XPT2046_PENIRQ_GPIO_PORT, &GPIO_InitStructure);
}


void XPT2046_SPI_SendData(uint8_t data) 
{
  	uint8_t i;   
    XPT2046_CS_LOW();

 
	for ( i = 0; i < 8; i ++ ) 
	{
        XPT2046_CLK_LOW();
        delay_us(5);
		if(data & 0x80) 
        {
            XPT2046_MOSI_1();
        } 
        else 
        {
            XPT2046_MOSI_0();
        }
        data <<= 1;
    
        XPT2046_CLK_HIGH();
        delay_us(5);
	}
 
}


uint16_t XPT2046_SPI_RecvData(void) 
{
    
    uint8_t i;
	uint16_t data = 0;
    
    
	for ( i = 0;i < 16; i++ ) 
	{
        XPT2046_CLK_LOW();
        delay_us(5);
      
        data <<= 1;
		if(XPT2046_MISO()) 
        {
            data |= 1;
        }
        else 
        {
            data |= 0;
        }
		XPT2046_CLK_HIGH();
        delay_us(5);   // wait busy
	}
	XPT2046_CS_HIGH(); // �ͷ�Ƭѡ�ź�
	return data>>4;
 
}

// ��ȡX ����Y ����Ĳ���ֵ
uint16_t XPT2046_ReadAdc(uint8_t channelCmd) 
{
    XPT2046_SPI_SendData (channelCmd);
    delay_us(6);
	XPT2046_CLK_LOW();
    delay_us(5);
    XPT2046_CLK_HIGH();
    delay_us(5);   // wait busy
    return XPT2046_SPI_RecvData();

}

//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
//xy:ָ�CMD_RDX/CMD_RDY��
//����ֵ:����������
#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
uint16_t TP_Read_XOY(uint8_t xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum = 0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)
    {
        buf[i]=XPT2046_ReadAdc(xy);		 		    
    }
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j = i+1;j < READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp = buf[i];
				buf[i] = buf[j];
				buf[j] = temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i< READ_TIMES - LOST_VAL;i++)
    {
        sum+=buf[i];
    }
	temp = sum / (READ_TIMES - 2*LOST_VAL);
    
	return temp;   
} 


/**
  * @brief  ��ȡ XPT2046 ��Xͨ����Yͨ����ADֵ��12 bit�������4096��
  * @param  sX_Ad �����Xͨ��ADֵ�ĵ�ַ
  * @param  sY_Ad �����Yͨ��ADֵ�ĵ�ַ
  * @retval ��
  */
uint8_t XPT2046_ReadAdc_XY ( uint16_t * x, uint16_t * y )  
{ 
    * x = TP_Read_XOY ( CMD_RDX );
	delay_us (1); 
	* y = TP_Read_XOY ( CMD_RDY ); 
    return 1;
}

//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 //��Χ 
uint8_t TP_Read_XY2(uint16_t *x, uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;

 	uint8_t flag;    
    flag = XPT2046_ReadAdc_XY(&x1, &y1);   

    if(flag == 0) 
    {
        return(0);
    }
    flag = XPT2046_ReadAdc_XY(&x2, &y2);	
    
    if(flag == 0)
    {
        return(0);   
    }
    //ǰ�����β�����+-50��
    if(((x2<=x1 && x1< (x2 + ERR_RANGE))||(x1 <= x2 && x2 < (x1+ERR_RANGE))) && (( y2 <= y1 && y1< (y2+ERR_RANGE))||(y1 <= y2 && (y2 < y1+ERR_RANGE))))
    {
        *x=(x1+x2)>>1;
        *y=(y1+y2)>>1;
        return 1;
    }
    else
    { 
        printf("x,y return 0;"); 
        return 0;	  
    }
}   

//////////////////////////////////////////////////////////////////////////////////		  
//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
u8 TP_Scan(u8 tp)
{			   
	if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel)//�а�������
	{
		if(tp)
        {
            TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//��ȡУ׼����������
        }
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//��ȡ��Ļ����
		{
           
	 		tp_dev.x[0] = tp_dev.xfac * tp_dev.x[0] + tp_dev.xoff;//�����ת��Ϊ��Ļ����
			tp_dev.y[0] = tp_dev.yfac * tp_dev.y[0] + tp_dev.yoff; 

            
            printf(" innnnnnnnnnnn = %d %d  %\r\n",tp_dev.x[0],tp_dev.y[0]);                
	 	} 
		if((tp_dev.sta & TP_PRES_DOWN) == 0)//֮ǰû�б�����
		{		 
			tp_dev.sta = TP_PRES_DOWN;
			tp_dev.x[4] = tp_dev.x[0];//��¼��һ�ΰ���ʱ������
			tp_dev.y[4] = tp_dev.y[0];  	   			 
		}
	}
    else
	{
		if(tp_dev.sta & TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta &= ~(1<<7);//��ǰ����ɿ�	
		}
        else//֮ǰ��û�б�����
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta & TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}	  


//����У׼����										    
void TP_Save_Adjdata(void)
{
    u8 data = 0x0a;
    W25Q128_Init();
   
    //��������
    SPI_FLASH_SectorErase(0);
    //д�봥��������־ 0x0a
    W25Q128_Write_Page(&data,SAVE_ADDR_BASE,1); //�ڿ�ʼ��д0X0A���У׼����
    //д�����µĴ�������
    W25Q128_Write_Page((uint8_t*)&tp_dev.xfac, SAVE_ADDR_BASE+1, 14); //ǿ�Ʊ���&tp_dev.xfac��ַ��ʼ��14���ֽ����ݣ������浽tp_dev.touchtype
}
//�õ�������EEPROM�����У׼ֵ
//����ֵ��1���ɹ���ȡ����
//        0����ȡʧ�ܣ�Ҫ����У׼
u8 TP_Get_Adjdata(void)
{					  
	uint8_t temp = 0;
    W25Q128_Init();
	W25Q128_Read(&temp,SAVE_ADDR_BASE, 1);//��ȡ�����,���Ƿ�У׼���� 	
    printf("temp ===== %d\r\n",temp);
	if(temp==0X0A)//�������Ѿ�У׼����			   
 	{ 
		W25Q128_Read((u8*)&tp_dev.xfac, SAVE_ADDR_BASE+1, 14);//��ȡ֮ǰ�����У׼���� 
        
        printf("tp_dev.xfac = %f\r\n",tp_dev.xfac);
        printf("tp_dev.xfac = %d\r\n",tp_dev.xoff);
        printf("tp_dev.xfac = %f\r\n",tp_dev.yfac);
        printf("tp_dev.xfac = %d\r\n",tp_dev.yoff);
        printf("tp_dev.xfac = %d\r\n",tp_dev.touchtype);
        
		if(tp_dev.touchtype == HORIZONTAL)//X,Y��������Ļ�෴
		{
			CMD_RDX=0X90;
			CMD_RDY=0XD0;	 
		}else				   //X,Y��������Ļ��ͬ
		{
			CMD_RDX=0XD0;
			CMD_RDY=0X90;	 
		}
        
		return 1;	 
	}
	return 0;
}


void TP_Adjust(uint8_t LCD_Mode)  
{ 
    uint16_t pos_temp[4][2]; //���껺��ֵ
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	double fac;
    char * pStr = 0;
    uint16_t outtime=0;
    char cStr [ 100 ];
    
    LCD_SetFont(&Font8x16);
    LCD_SetColors(BLUE, BLACK);
    
    //����ɨ�跽�򣬺���
    setScanOrigation ( LCD_Mode );
    ILI9341_Clear ( 0, 0, LCD_X_LENGTH, LCD_Y_LENGTH );       

    drowCrossInfo(1, 20, 20); 
 
    while(1) 
    {
        tp_dev.scan(1);//ɨ����������
    
        if((tp_dev.sta&0x80)==TP_PRES_DOWN)//����������һ��(��ʱ�����ɿ���.) 
        {    
            while(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel);
            outtime=0;
            tp_dev.sta&=~(1<<7);//��ǰ����Ѿ����������.
            pos_temp[cnt][0]=tp_dev.x[0];
            pos_temp[cnt][1]=tp_dev.y[0];      
            cnt++;	
            
            switch(cnt) 
            {
                case 1:
                    drowCrossInfo(2, LCD_X_LENGTH - 20, 20); 
                    
                    break;
                   
                case 2:
                    drowCrossInfo(3, 20, LCD_Y_LENGTH - 20); 
                    break;
            
                case 3:
                    drowCrossInfo(4, LCD_X_LENGTH - 20, LCD_Y_LENGTH - 20); 
                    break;
                
                case 4:
                    //�Ա����
                
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
                
                
                    
					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
					fac=(float)d1/d2;
        
                            
					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
					{
						cnt=0;
 				    	drowCrossInfo(1, 20, 20); 
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
                   
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	drowCrossInfo(1, 20, 20); 
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
                    
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	drowCrossInfo(1, 20, 20); 
						continue;
					}//��ȷ��
					//������
					tp_dev.xfac=(float)(LCD_X_LENGTH-40)/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					tp_dev.xoff=(LCD_X_LENGTH-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						              
					tp_dev.yfac=(float)(LCD_Y_LENGTH-40)/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					tp_dev.yoff=(LCD_Y_LENGTH - tp_dev.yfac * (pos_temp[2][1] + pos_temp[0][1])) / 2;//�õ�yoff  
         
                    LCD_SetTextColor(BLUE);
					ILI9341_Clear(0,0, LCD_X_LENGTH, LCD_Y_LENGTH);	//����
                    
                    pStr = "Adjuest Success!";
					sprintf(cStr,"%*c%s",((LCD_X_LENGTH/(((sFONT *)LCD_GetFont())->Width))-strlen(pStr))/2,' ',pStr);	
                    ILI9341_DispStringLine_EN (( LCD_Y_LENGTH >> 1 ) - (((sFONT *)LCD_GetFont())->Height), cStr ); 
					delay_ms(1000);
                    TP_Save_Adjdata();  
                    return;
            }
        }
        delay_ms(10);
		outtime++;
		if(outtime>1000)
		{
			TP_Get_Adjdata();
            printf("auto release!!\r\n");
			break;
	 	} 
    }
}


void drowCrossInfo(uint8_t i,uint16_t x, uint16_t y ) 
{
    char * pStr = 0;
    char cStr [ 100 ];
    ILI9341_Clear ( 0, 0, LCD_X_LENGTH, LCD_Y_LENGTH );       
        
    pStr = "Touch Calibrate ......";		
    //����ո񣬾�����ʾ
    sprintf(cStr,"%*c%s",((LCD_X_LENGTH/(((sFONT *)LCD_GetFont())->Width))-strlen(pStr))/2,' ',pStr);	
    ILI9341_DispStringLine_EN (LCD_Y_LENGTH >> 1, cStr );			

    //����ո񣬾�����ʾ 1
    sprintf ( cStr, "%*c%d",((LCD_X_LENGTH/(((sFONT *)LCD_GetFont())->Width)) -1)/2,' ', i);
    ILI9341_DispStringLine_EN (( LCD_Y_LENGTH >> 1 ) - (((sFONT *)LCD_GetFont())->Height), cStr ); 

    delay_ms (10);		                     //�ʵ�����ʱ���б�Ҫ

    ILI9341_DrawCross (x, y);  //��ʾУ���õġ�ʮ����
    
}
static void ILI9341_DrawCross ( uint16_t usX, uint16_t usY )
{
	ILI9341_DrawLine(usX - 10, usY, usX+10, usY);
	ILI9341_DrawLine(usX, usY - 10, usX, usY+10);	
}

uint8_t TP_Init(void) 
{
    if(tp_dev.touchtype == HORIZONTAL) 
    {
        CMD_RDX = 0X90;
        CMD_RDY = 0XD0;
    }
    else 
    {
        CMD_RDX = 0XD0;
        CMD_RDY = 0X90; 
    }
    

    XPT2046_ReadAdc_XY(&tp_dev.x[0],&tp_dev.y[0]);//��һ�ζ�ȡ��ʼ��	 
    
    if(TP_Get_Adjdata())
    {
        return 0;//�Ѿ�У׼
    }
    else			  		//δУ׼?
    { 										    
        ILI9341_Clear(0,0, LCD_X_LENGTH, LCD_Y_LENGTH);	//����
        TP_Adjust(LCD_SCAN_MODE);  		//��ĻУ׼  
    }	

    return 1;
}

void rtp_test(void)
{
	u8 key;  
    u16 tempX=0,tempY=0;
    Palette_Init(LCD_SCAN_MODE);
	while(1)
	{
	 	key=key_scan();
		tp_dev.scan(0); 		 
		if(tp_dev.sta & TP_PRES_DOWN)			//������������
		{	
            led_green_toggle();		
            printf(" in = %d %d %\r\n",tp_dev.x[0],tp_dev.y[0]);    
		 	if((tp_dev.x[0] < LCD_X_LENGTH) && (tp_dev.y[0] < LCD_Y_LENGTH))
			{	     
                Touch_Button_Down(tp_dev.x[0],tp_dev.y[0]);
               
                /*�������켣*/
                Draw_Trail(tempX, tempY, tp_dev.x[0], tp_dev.y[0], &brush);
                
                tempX=tp_dev.x[0];
                tempY=tp_dev.y[0];
			}
		}
        else 
        { 
            led_red_toggle();	
            Touch_Button_Up(tempX,tempY);	
            tempX = 0;
            tempY = 0;
        }  
		if(key==KEY0_PRES)	//KEY0����,��ִ��У׼����
		{
			ILI9341_Clear(0,0, LCD_X_LENGTH, LCD_Y_LENGTH);	//����
		    TP_Adjust(LCD_SCAN_MODE);  	//��ĻУ׼  
            Palette_Init(LCD_SCAN_MODE);
		}
	}
}

