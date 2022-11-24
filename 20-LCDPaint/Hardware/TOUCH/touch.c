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


// 默认横屏
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
	

  /* 开启GPIO时钟 */
  RCC_APB2PeriphClockCmd ( XPT2046_SPI_GPIO_CLK|XPT2046_PENIRQ_GPIO_CLK, ENABLE );

  /* 模拟SPI GPIO初始化 */          
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
   
  /* 拉低片选，选择XPT2046 */
  XPT2046_CS_LOW();		
								
	//触摸屏触摸信号指示引脚，不使用中断
  GPIO_InitStructure.GPIO_Pin = XPT2046_PENIRQ_GPIO_PIN;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
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
	XPT2046_CS_HIGH(); // 释放片选信号
	return data>>4;
 
}

// 读取X 或者Y 方向的采样值
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

//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
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
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j = i+1;j < READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
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
  * @brief  读取 XPT2046 的X通道和Y通道的AD值（12 bit，最大是4096）
  * @param  sX_Ad ：存放X通道AD值的地址
  * @param  sY_Ad ：存放Y通道AD值的地址
  * @retval 无
  */
uint8_t XPT2046_ReadAdc_XY ( uint16_t * x, uint16_t * y )  
{ 
    * x = TP_Read_XOY ( CMD_RDX );
	delay_us (1); 
	* y = TP_Read_XOY ( CMD_RDY ); 
    return 1;
}

//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 //误差范围 
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
    //前后两次采样在+-50内
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
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
u8 TP_Scan(u8 tp)
{			   
	if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel)//有按键按下
	{
		if(tp)
        {
            TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//读取校准的物理坐标
        }
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//读取屏幕坐标
		{
           
	 		tp_dev.x[0] = tp_dev.xfac * tp_dev.x[0] + tp_dev.xoff;//将结果转换为屏幕坐标
			tp_dev.y[0] = tp_dev.yfac * tp_dev.y[0] + tp_dev.yoff; 

            
            printf(" innnnnnnnnnnn = %d %d  %\r\n",tp_dev.x[0],tp_dev.y[0]);                
	 	} 
		if((tp_dev.sta & TP_PRES_DOWN) == 0)//之前没有被按下
		{		 
			tp_dev.sta = TP_PRES_DOWN;
			tp_dev.x[4] = tp_dev.x[0];//记录第一次按下时的坐标
			tp_dev.y[4] = tp_dev.y[0];  	   			 
		}
	}
    else
	{
		if(tp_dev.sta & TP_PRES_DOWN)//之前是被按下的
		{
			tp_dev.sta &= ~(1<<7);//标记按键松开	
		}
        else//之前就没有被按下
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta & TP_PRES_DOWN;//返回当前的触屏状态
}	  


//保存校准参数										    
void TP_Save_Adjdata(void)
{
    u8 data = 0x0a;
    W25Q128_Init();
   
    //擦除扇区
    SPI_FLASH_SectorErase(0);
    //写入触摸参数标志 0x0a
    W25Q128_Write_Page(&data,SAVE_ADDR_BASE,1); //在开始，写0X0A标记校准过了
    //写入最新的触摸参数
    W25Q128_Write_Page((uint8_t*)&tp_dev.xfac, SAVE_ADDR_BASE+1, 14); //强制保存&tp_dev.xfac地址开始的14个字节数据，即保存到tp_dev.touchtype
}
//得到保存在EEPROM里面的校准值
//返回值：1，成功获取数据
//        0，获取失败，要重新校准
u8 TP_Get_Adjdata(void)
{					  
	uint8_t temp = 0;
    W25Q128_Init();
	W25Q128_Read(&temp,SAVE_ADDR_BASE, 1);//读取标记字,看是否校准过！ 	
    printf("temp ===== %d\r\n",temp);
	if(temp==0X0A)//触摸屏已经校准过了			   
 	{ 
		W25Q128_Read((u8*)&tp_dev.xfac, SAVE_ADDR_BASE+1, 14);//读取之前保存的校准数据 
        
        printf("tp_dev.xfac = %f\r\n",tp_dev.xfac);
        printf("tp_dev.xfac = %d\r\n",tp_dev.xoff);
        printf("tp_dev.xfac = %f\r\n",tp_dev.yfac);
        printf("tp_dev.xfac = %d\r\n",tp_dev.yoff);
        printf("tp_dev.xfac = %d\r\n",tp_dev.touchtype);
        
		if(tp_dev.touchtype == HORIZONTAL)//X,Y方向与屏幕相反
		{
			CMD_RDX=0X90;
			CMD_RDY=0XD0;	 
		}else				   //X,Y方向与屏幕相同
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
    uint16_t pos_temp[4][2]; //坐标缓存值
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	double fac;
    char * pStr = 0;
    uint16_t outtime=0;
    char cStr [ 100 ];
    
    LCD_SetFont(&Font8x16);
    LCD_SetColors(BLUE, BLACK);
    
    //设置扫描方向，横屏
    setScanOrigation ( LCD_Mode );
    ILI9341_Clear ( 0, 0, LCD_X_LENGTH, LCD_Y_LENGTH );       

    drowCrossInfo(1, 20, 20); 
 
    while(1) 
    {
        tp_dev.scan(1);//扫描物理坐标
    
        if((tp_dev.sta&0x80)==TP_PRES_DOWN)//按键按下了一次(此时按键松开了.) 
        {    
            while(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel);
            outtime=0;
            tp_dev.sta&=~(1<<7);//标记按键已经被处理过了.
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
                    //对边相等
                
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,2的距离
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
                
                
                    
					d2=sqrt(tem1+tem2);//得到3,4的距离
					fac=(float)d1/d2;
        
                            
					if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
					{
						cnt=0;
 				    	drowCrossInfo(1, 20, 20); 
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					fac=(float)d1/d2;
                   
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	drowCrossInfo(1, 20, 20); 
						continue;
					}//正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
                    
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	drowCrossInfo(1, 20, 20); 
						continue;
					}//正确了
					//计算结果
					tp_dev.xfac=(float)(LCD_X_LENGTH-40)/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					tp_dev.xoff=(LCD_X_LENGTH-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						              
					tp_dev.yfac=(float)(LCD_Y_LENGTH-40)/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					tp_dev.yoff=(LCD_Y_LENGTH - tp_dev.yfac * (pos_temp[2][1] + pos_temp[0][1])) / 2;//得到yoff  
         
                    LCD_SetTextColor(BLUE);
					ILI9341_Clear(0,0, LCD_X_LENGTH, LCD_Y_LENGTH);	//清屏
                    
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
    //插入空格，居中显示
    sprintf(cStr,"%*c%s",((LCD_X_LENGTH/(((sFONT *)LCD_GetFont())->Width))-strlen(pStr))/2,' ',pStr);	
    ILI9341_DispStringLine_EN (LCD_Y_LENGTH >> 1, cStr );			

    //插入空格，居中显示 1
    sprintf ( cStr, "%*c%d",((LCD_X_LENGTH/(((sFONT *)LCD_GetFont())->Width)) -1)/2,' ', i);
    ILI9341_DispStringLine_EN (( LCD_Y_LENGTH >> 1 ) - (((sFONT *)LCD_GetFont())->Height), cStr ); 

    delay_ms (10);		                     //适当的延时很有必要

    ILI9341_DrawCross (x, y);  //显示校正用的“十”字
    
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
    

    XPT2046_ReadAdc_XY(&tp_dev.x[0],&tp_dev.y[0]);//第一次读取初始化	 
    
    if(TP_Get_Adjdata())
    {
        return 0;//已经校准
    }
    else			  		//未校准?
    { 										    
        ILI9341_Clear(0,0, LCD_X_LENGTH, LCD_Y_LENGTH);	//清屏
        TP_Adjust(LCD_SCAN_MODE);  		//屏幕校准  
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
		if(tp_dev.sta & TP_PRES_DOWN)			//触摸屏被按下
		{	
            led_green_toggle();		
            printf(" in = %d %d %\r\n",tp_dev.x[0],tp_dev.y[0]);    
		 	if((tp_dev.x[0] < LCD_X_LENGTH) && (tp_dev.y[0] < LCD_Y_LENGTH))
			{	     
                Touch_Button_Down(tp_dev.x[0],tp_dev.y[0]);
               
                /*处理描绘轨迹*/
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
		if(key==KEY0_PRES)	//KEY0按下,则执行校准程序
		{
			ILI9341_Clear(0,0, LCD_X_LENGTH, LCD_Y_LENGTH);	//清屏
		    TP_Adjust(LCD_SCAN_MODE);  	//屏幕校准  
            Palette_Init(LCD_SCAN_MODE);
		}
	}
}

