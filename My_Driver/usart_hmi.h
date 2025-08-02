#ifndef __USART_HMI_H
#define __USART_HMI_H

#include "main.h"
/***********************************************************************************************************/



extern u8 page_number;
extern char usart_hmi_SendBuffer[50];
extern unsigned char usart_hmi_RecWord[4];
extern unsigned int usart_hmi_RecNum;
extern unsigned char usart_hmi_WordAvalable;


void USART_HMI_Init(uint32_t baudRate);
void USART_HMI_Config(void);
void USART_HMI_SendByte(uint8_t ch);
void USART_HMI_SendString(char* str);
uint8_t USART_HMI_RecieveByte();
void USART_HMI_ITRecieveWord();

void USART_HMI_Wait_Until_Rst();
void USART_HMI_Addt_Cmd(const char* objid,uint8_t ch,uint8_t* DataPtr,uint16_t DataSize);
int USART_HMI_ReadNum(const char* obj);

#define USART_HMI_Printf(...)\
sprintf(usart_hmi_SendBuffer,__VA_ARGS__);\
USART_HMI_SendString(usart_hmi_SendBuffer)




#endif
