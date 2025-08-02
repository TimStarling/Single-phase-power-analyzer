#include "usart_hmi.h"


u8 page_number = 0;
char usart_hmi_SendBuffer[50];//�������黺����
unsigned char usart_hmi_RecWord[4];//�Ӵ������Ľ����ֽ�
unsigned int usart_hmi_RecNum = 0;//�жϺ����м�¼���ܵ����ֽ����������ֵΪ4ʱ���������ϣ��رս����ж�ֱ����һ����������
unsigned char usart_hmi_WordAvalable = 0;//��������֮���ֵ��1
/*
p3.2 RXD
P3.3 TXD
*/


void USART_HMI_Init(uint32_t baudRate)
{
	
#ifdef EUSCI_A_UART_7_BIT_LEN
  // �̼���v3_40_01_02
  // Ĭ��SMCLK 48MHz ������ 115200
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
  eusci_calcBaudDividers((eUSCI_UART_ConfigV1 *)&uartConfig, baudRate); // ���ò�����
#else
  // �̼���v3_21_00_05
  // Ĭ��SMCLK 48MHz ������ 115200
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
  eusci_calcBaudDividers((eUSCI_UART_Config *)&uartConfig, baudRate); // ���ò�����
#endif

  MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
  MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig);
  MAP_UART_enableModule(EUSCI_A2_BASE);

  UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT); // 5.������������ж�
  MAP_Interrupt_enableInterrupt(INT_EUSCIA2);                          // 6.�������ڶ˿��ж�
}

void USART_HMI_SendByte(uint8_t ch)
{
	UART_transmitData(EUSCI_A2_BASE,ch);

    //�ȴ����ͼĴ���Ϊ��
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
������飺����������ռ�õĴ��ڵ��жϽ��գ�������4���ֽ�֮����Զ��ر��жϣ�
4���ֽڰ��Ⱥ�˳��洢��usart_hmi_RecWord[0]~[3]�У����Կ��ƴ�������Ƭ�����������Ǳ�����4���ֽ�һ֡����ʽ��
��ʹ�Ӵ�������Ҫ��������ֻ��һ���ֽڣ�ҲҪ�ں������FF FF FF������һ�δ����ĸ��ֽڡ���Ƭ���򴮿���������û�д˾���
*/
void USART_HMI_ITRecieveWord()
{
	usart_hmi_RecNum = 0;
	usart_hmi_WordAvalable = 0;
	
	*(uint32_t*)usart_hmi_RecWord = 0;
  UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT); // 5.������������ж�
}

//�������ȴ������������������������ᷢ��0x88,0xff,0xff,0xff,���յ����ĸ��ֽ�֮ǰ�������Ῠ���ڴκ�����
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
������飺�򴮿������οؼ����Ͳ���
����˵����
const char* objid�����οؼ���id
uint8_t ch�����οؼ���ͨ��
uint8_t* DataPtr��Ҫ���ݵĲ�������
uint16_t DataSize��Ҫ���ݵĲ��ε���
���þ�����
{
	int i;
	uint8_t Array[256];//����һ���洢
	
	for(i=0;i<256;i++)//��Array���鸳ֵ
	{
		Array[i] = ��ֵ;
	}

	USART_HMI_Addt_Cmd("s0,id",0,Array,256);//��Array�����еĲ��η��͸�������s0��ͨ��0��
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
	 //ȷ��͸������������Ӱ����һ��ָ��
 USART_HMI_Printf("\x01\xff\xff\xff");
}



/*
������飺
��ȡ�����������ֿؼ�ֵ
����˵����
const char* obj��Ҫ��ֵ�����ֿؼ�
����ֵ�����ֿؼ�����ֵ
*/
int USART_HMI_ReadNum(const char* obj)
{
    USART_HMI_ITRecieveWord();//ʹ���ж�
    USART_HMI_Printf("prints %s.val,4\xff\xff\xff",obj);//���ƴ�����������ֿؼ�ֵ
	//while(usart_hmi_WordAvalable != 1);//�ȴ���Ƭ���������
	
	return *(int*)usart_hmi_RecWord;
}





// �жϷ�����
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
				UART_disableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT); // 5.�رմ�������ж�

			}
	}

}


