#ifndef _MYDMA_H_

#define _MYDMA_H_

#include "sys.h"


void DMA_init(u32 fromAddr,u32 toAddr,u16 count);
void start_Transt(void);

#endif
