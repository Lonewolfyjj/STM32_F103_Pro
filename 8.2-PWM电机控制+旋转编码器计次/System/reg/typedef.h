#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#define RCC_BASE_ADDRESS 0x40021000


#define GPIOA_BASE_ADDRESS 0x40010800 
#define GPIOB_BASE_ADDRESS 0X40010C00 
#define GPIOC_BASE_ADDRESS 0x40011000
#define GPIOD_BASE_ADDRESS 0x40011400
#define GPIOE_BASE_ADDRESS 0x40011800
#define GPIOF_BASE_ADDRESS 0x40011C00
#define GPIOG_BASE_ADDRESS 0x40012000
#define EXIT_BASE_ADDRESS  0x40010400
#define AFIO_BASE_ADDRESS  0x40010000

typedef struct {
	volatile unsigned int CR;
	volatile unsigned int CFGR;
	volatile unsigned int CIR;
	volatile unsigned int APB2RSTR; // APB2ÍâÉè¸´Î»¼Ä´æÆ÷
	volatile unsigned int APB1RSTR; 
  volatile unsigned int AHBENR;
	volatile unsigned int APB2ENR;
	volatile unsigned int APB1ENR;
	volatile unsigned int BDCR;
	volatile unsigned int CSR;
} RCC_TYPE_DEF;


typedef struct {
	volatile unsigned int CRL;
	volatile unsigned int CRH;
	volatile unsigned int IDR;
	volatile unsigned int ODR;
	volatile unsigned int BSRR;
	volatile unsigned int BRR;
	volatile unsigned int LCKR;
} GPIO_TYPE_DEF;


typedef struct {
  volatile unsigned int IMR;
	volatile unsigned int EMR;
	volatile unsigned int RTSR;
	volatile unsigned int FTSR;
	volatile unsigned int SWIER;
	volatile unsigned int PR;
} EIXT_TYPE_DEF;

typedef struct {
	volatile unsigned int EVCR;
	volatile unsigned int MAPR;
	volatile unsigned int EXTICR1;
  volatile unsigned int EXTICR2;
	volatile unsigned int EXTICR3;
	volatile unsigned int EXTICR4;
	unsigned int RESERVED0;
  volatile unsigned int MAPR2;
}AFIO_TYPE_DEF;


#define RCC1    ((RCC_TYPE_DEF *) RCC_BASE_ADDRESS)
#define GPIOB1  ((GPIO_TYPE_DEF *) GPIOB_BASE_ADDRESS)
#define GPIOE1  ((GPIO_TYPE_DEF *) GPIOE_BASE_ADDRESS)
#define EXIT1    ((EIXT_TYPE_DEF *) EXIT_BASE_ADDRESS)
#define AFIO1   ((AFIO_TYPE_DEF *) AFIO_BASE_ADDRESS)

#endif
