#ifndef _MYDMA_H_

#define _MYDMA_H_

#include "sys.h"


extern u8 data1[];
extern u8 data2[];

void DMA_init(void);
void start_Transt(u32 fromAddr,u32 toAddr,u16 count);

#endif
