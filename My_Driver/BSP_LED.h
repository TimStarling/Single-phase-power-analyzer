#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "main.h"

void LED1_Init(void);
void LED1_OC(u8 LED1_Val);//CLOSE 0,OPEN 1,TOGGLE 2
void LED2_Init(void);
void LED2_OC(uint8_t LED2_Val);

#endif

