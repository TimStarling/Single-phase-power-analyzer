#include "BSP_LED.h"

void LED1_Init(void)
{
	GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
}

void LED1_OC(u8 LED1_Val)
{
	if(LED1_Val == 1)
		GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
	else if(LED1_Val == 2)
		GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
	else
		GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);
}

void LED2_Init(void)
{
	GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN0);
	GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
	GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN2);
}

void LED2_OC(uint8_t LED2_Val)
{
	if(LED2_Val&1)
		GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
	else
		GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
	
	if(LED2_Val&2)
		GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
	else
		GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
	
		if(LED2_Val&4)
		GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
	else
		GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
}

