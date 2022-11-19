#ifndef  _FONT_H_
#define  _FONT_H_
#include "ili9341.h"


#define      WIDTH_CH_CHAR		                32	    //中文字符宽度 
#define      HEIGHT_CH_CHAR		              	32		  //中文字符高度 

typedef struct _tFont
{    
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
  
} sFONT;

extern sFONT Font24x32;
extern sFONT Font16x24;
extern sFONT Font8x16;




uint8_t * GetGBKCode_from_sd ( uint8_t * pBuffer, uint16_t c);

//static const uint8_t zi[SIZE/8*SIZE] = {

//0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x02,0x80,0x02,0x80,
//0x04,0x40,0x04,0x40,0x08,0x20,0x08,0x20,0x10,0x10,0x20,0x08,0x40,0x04,0x80,0x02,/*"人",0*/

//};

#endif
