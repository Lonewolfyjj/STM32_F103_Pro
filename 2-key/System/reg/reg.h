#ifndef _REG_H_
#define _REG_H_

	#define RCC_APB2ENR		*((unsigned volatile int*)0x40021018)//RCC_APB2ENR 0x40021000 + 0x18
	#define GPIOB_CRL		  *((unsigned volatile int*)0x40010C00)//GPIOB_CRL     0X40010C00 + 0X00
	#define	GPIOB_ODR		  *((unsigned volatile int*)0x40010C0C)//GPIOB_ODR     0X40010C00 + 0X0C
		
	#define GPIOE_CRL		  *((unsigned volatile int*)0x40011800)//GPIOB_CRL     0x40011800 + 0X000
	#define	GPIOE_ODR		  *((unsigned volatile int*)0x4001180C)//GPIOB_ODR     0x4001180C + 0X0C
		
	#define BA(addr,n)   *((unsigned volatile int *) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF)<<5) + (n<<2)))

#endif
