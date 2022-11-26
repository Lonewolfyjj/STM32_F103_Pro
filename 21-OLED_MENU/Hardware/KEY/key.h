#ifndef _KEY_H_
#define _KEY_H_
  
	void key_init(void);
	unsigned int key_scan(void);
	#define KEY0_PRES 	1	//KEY0按下
	#define KEY1_PRES	2	//KEY1按下 
    #define KEY2_PRES	3	//KEY1按下 
    #define KEY3_PRES	4	//KEY1按下 

	#define NO_KEYPRESS 0xff
	
    #define GPIOE_IDR_Address 0x40011808
	#define PEin1(n) BA(GPIOE_IDR_Address,n)
    
	#define KEY0 PEin1(4)
	#define KEY1 PEin1(3)
    #define KEY2 PEin1(2)
	#define KEY3 PEin1(1)
#endif
