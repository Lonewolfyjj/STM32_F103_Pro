#ifndef _KEY_H_
#define _KEY_H_
  
	void key_init(void);
	unsigned int key_scan(void);
	#define KEY0_PRES 	1	//KEY0����
	#define KEY1_PRES	2	//KEY1���� 
	#define NO_KEYPRESS 0xff
	
  #define GPIOE_IDR_Address 0x40011808
	#define PEin1(n) BA(GPIOE_IDR_Address,n)
	
	#define KEY1 PEin1(3)
	#define KEY0 PEin1(4)
#endif
