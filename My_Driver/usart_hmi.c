#include "usart_hmi.h"


u8 page_number = 0;
char usart_hmi_SendBuffer[50];//发送数组缓冲区
unsigned char usart_hmi_RecWord[4];//从串口屏的接受字节
unsigned int usart_hmi_RecNum = 0;//中断函数中记录接受到的字节数，当这个值为4时代表接受完毕，关闭接受中断直到下一次主动开启
unsigned char usart_hmi_WordAvalable = 0;//当接受完之后此值置1
/*
p3.2 RXD
P3.3 TXD
*/


void USART_HMI_Init(uint32_t baudRate)
{
	
#ifdef EUSCI_A_UART_7_BIT_LEN
  // 固件库v3_40_01_02
  // 默认SMCLK 48MHz 比特率 115200
  const eUSCI_UART_ConfigV1 uartConfig =
      {
          EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source
          26,                                            // BRDIV = 26
          0,                                             // UCxBRF = 0
          111,                                           // UCxBRS = 111
          EUSCI_A_UART_NO_PARITY,                        // No Parity
          EUSCI_A_UART_LSB_FIRST,                        // LSB First
          EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
          EUSCI_A_UART_MODE,                             // UART mode
          EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
          EUSCI_A_UART_8_BIT_LEN                         // 8 bit data length
      };
  eusci_calcBaudDividers((eUSCI_UART_ConfigV1 *)&uartConfig, baudRate); // 配置波特率
#else
  // 固件库v3_21_00_05
  // 默认SMCLK 48MHz 比特率 115200
  const eUSCI_UART_Config uartConfig =
      {
          EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source
          26,                                            // BRDIV = 26
          0,                                             // UCxBRF = 0
          111,                                           // UCxBRS = 111
          EUSCI_A_UART_NO_PARITY,                        // No Parity
          EUSCI_A_UART_LSB_FIRST,                        // MSB First
          EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
          EUSCI_A_UART_MODE,                             // UART mode
          EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
      };
  eusci_calcBaudDividers((eUSCI_UART_Config *)&uartConfig, baudRate); // 配置波特率
#endif

  MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
  MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig);
  MAP_UART_enableModule(EUSCI_A2_BASE);

  UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT); // 5.开启串口相关中断
  MAP_Interrupt_enableInterrupt(INT_EUSCIA2);                          // 6.开启串口端口中断
}

void USART_HMI_SendByte(uint8_t ch)
{
	UART_transmitData(EUSCI_A2_BASE,ch);

    //等待发送寄存器为空
  while (!EUSCI_A_UART_BUSY);
}

void USART_HMI_SendString(char* str)
{
    uint32_t k = 0;
    do
    {
        USART_HMI_SendByte(*(str + k) );
        k++;
    } while ( *(str + k) != '\0' );
    
    while (!EUSCI_A_UART_BUSY);
}

uint8_t USART_HMI_RecieveByte()
{
   while (!EUSCI_A_UART_BUSY)
	{
		;
	}
	return UART_receiveData(EUSCI_A2_BASE);
}

/*
函数简介：开启串口屏占用的串口的中断接收，接收完4个字节之后会自动关闭中断，
4个字节按先后顺序存储到usart_hmi_RecWord[0]~[3]中，所以控制串口屏向单片机发送数据是必须是4个字节一帧的形式，
即使从串口屏需要读的数据只有一个字节，也要在后面添加FF FF FF以满足一次传输四个字节。单片机向串口屏发送则没有此拘束
*/
void USART_HMI_ITRecieveWord()
{
	usart_hmi_RecNum = 0;
	usart_hmi_WordAvalable = 0;
	
	*(uint32_t*)usart_hmi_RecWord = 0;
  UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT); // 5.开启串口相关中断
}

//函数：等待串口屏重启，串口屏重启会发送0x88,0xff,0xff,0xff,接收到这四个字节之前串口屏会卡死在次函数里
void USART_HMI_Wait_Until_Rst()
{
	do
	{
		USART_HMI_ITRecieveWord();
		while(usart_hmi_WordAvalable != 1);
	}while(usart_hmi_RecWord[0] != 0x88);
	return;
}

/*
函数简介：向串口屏波形控件发送波形
参数说明：
const char* objid：波形控件的id
uint8_t ch：波形控件的通道
uint8_t* DataPtr：要传递的波形数据
uint16_t DataSize：要传递的波形点数
调用举例：
{
	int i;
	uint8_t Array[256];//定义一个存储
	
	for(i=0;i<256;i++)//给Array数组赋值
	{
		Array[i] = 赋值;
	}

	USART_HMI_Addt_Cmd("s0,id",0,Array,256);//将Array数组中的波形发送给串口屏s0的通道0；
}
*/
void USART_HMI_Addt_Cmd(const char* objid,uint8_t ch,uint8_t* DataPtr,uint16_t DataSize)
{
	uint16_t i;
	USART_HMI_ITRecieveWord();
	USART_HMI_Printf("addt %s,%d,%d\xff\xff\xff",objid,ch,DataSize);
	while(usart_hmi_WordAvalable != 1);
	
	if(usart_hmi_RecWord[0] == 0xFE)
	{
		USART_HMI_ITRecieveWord();
		for(i=0;i<DataSize;i++)
		{
			USART_HMI_Printf("%c",DataPtr[i]);
		}
		while(usart_hmi_WordAvalable != 1);
	
	}
//	else 
//	{
//		while(1);
//	}
	 //确保透传结束，以免影响下一条指令
 USART_HMI_Printf("\x01\xff\xff\xff");
}



/*
函数简介：
读取串口屏的数字控件值
参数说明：
const char* obj：要读值的数字控件
返回值：数字控件的数值
*/
int USART_HMI_ReadNum(const char* obj)
{
    USART_HMI_ITRecieveWord();//使能中断
    USART_HMI_Printf("prints %s.val,4\xff\xff\xff",obj);//控制串口屏输出数字控件值
	//while(usart_hmi_WordAvalable != 1);//等待单片机接收完毕
	
	return *(int*)usart_hmi_RecWord;
}





// 中断服务函数
void EUSCIA2_IRQHandler()
{
	uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

	if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
	{
			usart_hmi_RecWord[usart_hmi_RecNum] = USART_HMI_RecieveByte();
			usart_hmi_RecNum++;
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='a')
			{
				Page0_Change = 1;
				page_number = 0;
			}
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='b')
				page_number = 1;
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='c')
				page_number = 2;
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='d')
				page_number = 3;
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='e')
				page_number = 4;
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='f')
				page_number = 5;
			
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='g')
			{
				Relay_Open = 0;
				Relay_Flag = true;
			}
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='h')
			{
				Relay_Open = 1;
				Relay_Flag = true;
			}
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='i')
			{
				Relay_Open = 2;
				Relay_Flag = true;
			}
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='j')
			{
				Relay_Open = 3;
				Relay_Flag = true;
			}
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='k')
				Relay_Auto = true;
			if(usart_hmi_RecWord[usart_hmi_RecNum-1]=='l')
				Relay_Auto = false;
			
			if(usart_hmi_RecNum == 4)
			{
				usart_hmi_WordAvalable = 1;
				UART_disableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT); // 5.关闭串口相关中断

			}
	}

}


