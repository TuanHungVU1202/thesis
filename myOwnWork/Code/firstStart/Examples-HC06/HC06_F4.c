/*#############################################################
Driver name	    : HC06_F4.c
Author 					: Grant Phillips
Date Modified   : 25/09/2014
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)
Tested On       : STM32F4-Discovery

Description			: Provides a library to access the USART2 module
									on the STM32F4-Discovery to establish serial
									communication with a remote device (e.g. PC)
									using the HC-06 Bluetooth module.

Requirements    : * STM32F4-Discovery Board
									* HC-06 Bluetooth module

Functions				: HC06_Init
									HC06_PutChar
									HC06_PutStr
									HC06_ClearRxBuffer
									HC06_Test
									HC06_SetBaud
									HC06_SetName
									HC06_SetPin
											
Special Note(s) : In this driver PA2 is used as USART2_TX and
									PA3 as USART2_RX.  Any other UART and pins can
									be used, just change the relevant GPIO config-
									urations and occurrences of USART2 to the new
									UART/USART number.
##############################################################*/

#include <stdio.h>
#include <string.h>

#define HC06_RX_BUFFER_LENGTH	40																		//maximum number of characters to hold in the receive buffer
#define	HC06_TIMEOUT_MAX			94000																	//timeout value for waiting for a response from the HC-06 (+/- 2 secs)

 
void HC06_Init(uint32_t speed);	
void HC06_PutChar(unsigned char c);
void HC06_PutStr(char *str);
void HC06_ClearRxBuffer(void);
uint8_t HC06_Test(void);
uint8_t HC06_SetBaud(uint32_t speed);
uint8_t HC06_SetName(char *name);
uint8_t HC06_SetPin(char *pin);
void HC06_Delay(uint32_t delay);
	



char HC06_rx_buffer[HC06_RX_BUFFER_LENGTH];													//used by the IRQ handler
uint8_t HC06_rx_counter = 0; 																				//used by the IRQ handler
char HC06_msg[HC06_RX_BUFFER_LENGTH];																//variable that contains the latest string received on the RX pin
uint8_t new_HC06_msg = 0;																						//flag variable to indicate if there is a new message to be serviced



/*********************************************************************************************
Function name   : HC06_Init
Author 					: Andrei Istodorescu
Date Modified   : 02/08/2014 (Grant Phillips)
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Initializes the HC-06 Bluetooth module

Special Note(s) : NONE

Parameters			: speed		-	32-bit value to set the baud rate
Return value		: NONE
*********************************************************************************************/
void HC06_Init(uint32_t speed)
{
	char buf[20];
	
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* USARTx configured as follow:
				- BaudRate = speed parameter above
				- Word Length = 8 Bits
				- One Stop Bit
				- No parity
				- Hardware flow control disabled (RTS and CTS signals)
				- Receive and transmit enabled
	*/
		
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	/* USART configuration */
	USART_InitStructure.USART_BaudRate = speed;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect PA2 to USART2_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

	/* Connect PA3 to USART2_Rx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);
	
  /* Enable the UART3 Receive interrupt: this interrupt is generated when the
  UART5 receive data register is not empty */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
 
  /* Enable the USART3 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Flush USART2 Tx Buffer */
	sprintf(buf, "\n\r");
	HC06_PutStr(buf);
}



/*********************************************************************************************
Function name   : UART5_IRQHandler
Author 					: Andrei Istodorescu
Date Modified   : 05/08/2014 (Grant Phillips)
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Handles the interrupt when a new byte is received on the RX pin.  It works  
									on the assumption that the incoming message will be terminated by a 
									LF (0x10) character.

Special Note(s) : NONE

Parameters			: NONE
Return value		: NONE
*********************************************************************************************/
void USART2_IRQHandler(void)
{	
	int x;
	//char buf[40];
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
		/* Read one byte from the receive data register */
    HC06_rx_buffer[HC06_rx_counter] = USART_ReceiveData(USART2); 	
 
		//printf("%c", HC06_rx_buffer[HC06_rx_counter]);					//debug code
		
		/* if the last character received is the LF ('\r' or 0x0a) character OR if the HC06_RX_BUFFER_LENGTH (40) value has been reached ...*/
    if((HC06_rx_counter + 1 == HC06_RX_BUFFER_LENGTH) || (HC06_rx_buffer[HC06_rx_counter] == 0x0a))
    {
      new_HC06_msg = 1;
			for(x=0; x <= HC06_rx_counter; x++)											//copy each character in the HC06_rx_buffer to the HC06_msg variable
				HC06_msg[x] = HC06_rx_buffer[x];
			HC06_msg[x-1] = '\0';																		//terminate with NULL character
      memset(HC06_rx_buffer, 0, HC06_RX_BUFFER_LENGTH);				//clear HC06_rx_buffer
      HC06_rx_counter = 0;			
    }
    else
    {
			HC06_rx_counter++;
    }
  }
} 



/*********************************************************************************************
Function name   : HC06_PutStr
Author 					: Grant Phillips
Date Modified   : 05/08/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Writes a string to the HC-06 Bluetooth module.

Special Note(s) : NONE

Parameters			: str			-	string (char array) to print

Return value		: NONE
*********************************************************************************************/
void HC06_PutStr(char *str)
{
	unsigned int i=0;

	do
	{
		HC06_PutChar(str[i]);
		i++;
	}while(str[i]!='\0');
}



/*********************************************************************************************
Function name   : HC06_PutChar
Author 					: Grant Phillips
Date Modified   : 05/08/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Writes a character to the HC-06 Bluetooth module.

Special Note(s) : NONE

Parameters			: ch				-	character to print

Return value		: NONE
*********************************************************************************************/
void HC06_PutChar(unsigned char ch)
{
	/* Put character on the serial line */
	USART_SendData(USART2, (ch & (uint16_t)0x01FF));
	/* Loop until transmit data register is empty */
	while( !(USART2->SR & 0x00000040) );
}



/*********************************************************************************************
Function name   : HC06_ClearRxBuffer
Author 					: Grant Phillips
Date Modified   : 05/08/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Clears the software Rx buffer for the HC-06.

Special Note(s) : NONE

Parameters			: ch				-	character to print

Return value		: NONE
*********************************************************************************************/
void HC06_ClearRxBuffer(void)
{
	memset(HC06_rx_buffer, 0, HC06_RX_BUFFER_LENGTH);		//clear HC06_rx_buffer
	HC06_rx_counter = 0;																//reset the Rx buffer counter
	new_HC06_msg = 0;																		//reset new message flag
}



/*********************************************************************************************
Function name   : HC06_Test
Author 					: Grant Phillips
Date Modified   : 06/08/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Tests if there is communications with the HC-06.

Special Note(s) : NONE

Parameters			: NONE

Return value		: 0		-		Success
									1		-		Timeout error; not enough characters received for "OK" message
									2		-		enough characters received, but incorrect message
*********************************************************************************************/
uint8_t HC06_Test(void)
{
	uint32_t timeout = HC06_TIMEOUT_MAX;
	
	HC06_ClearRxBuffer();																//clear rx buffer
	HC06_PutStr("AT");																	//AT command for TEST COMMUNICATIONS
	while(HC06_rx_counter < 2)													//wait for "OK" - i.e. waiting for 2 characters
	{
		timeout--;
		HC06_Delay(1000);																	//wait +/- 100us just to give interrupt time to service incoming message
		if (timeout == 0) 
			return 0x01;																		//if the timeout delay is exeeded, exit with error code
	}
	if(strcmp(HC06_rx_buffer, "OK") == 0)
		return 0x00;																			//success
	else
		return 0x02;																			//unknown return AT msg from HC06
}



/*********************************************************************************************
Function name   : HC06_SetBaud
Author 					: Grant Phillips
Date Modified   : 06/08/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Set the default Baud rate for the HC-06.

Special Note(s) : NONE

Parameters			: speed - 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 or 230400

Return value		: 0		-		Success
									1		-		Incorrect speed selected/typed
									2		-		Timeout error; not enough characters received for "OKxxxx" message
									3		-		enough characters received, but incorrect message
*********************************************************************************************/
uint8_t HC06_SetBaud(uint32_t speed)
{
	uint32_t timeout = HC06_TIMEOUT_MAX;
	char buf[20];
	
	HC06_ClearRxBuffer();																//clear rx buffer
	//AT command for SET BAUD speed
	if(speed == 1200)
	{
		strcpy(buf, "OK1200");
		HC06_PutStr("AT+BAUD1");													
	}
	else if(speed == 2400)
	{
		strcpy(buf, "OK2400");
		HC06_PutStr("AT+BAUD2");													
	}
	else if(speed == 4800)
	{
		strcpy(buf, "OK4800");
		HC06_PutStr("AT+BAUD3");													
	}
	else if(speed == 9600)
	{
		strcpy(buf, "OK9600");
		HC06_PutStr("AT+BAUD4");													
	}
	else if(speed == 19200)
	{
		strcpy(buf, "OK19200");
		HC06_PutStr("AT+BAUD5");													
	}
	else if(speed == 38400)
	{
		strcpy(buf, "OK38400");
		HC06_PutStr("AT+BAUD6");													
	}
	else if(speed == 57600)
	{
		strcpy(buf, "OK57600");
		HC06_PutStr("AT+BAUD7");													
	}
	else if(speed == 115200)
	{
		strcpy(buf, "OK115200");
		HC06_PutStr("AT+BAUD8");													
	}
	else if(speed == 230400)
	{
		strcpy(buf, "OK230400");
		HC06_PutStr("AT+BAUD9");													
	}
	else
	{
		return 0x01;																			//error - incorrect speed
	}

	while(HC06_rx_counter < strlen(buf))								//wait for "OK" message
	{
		timeout--;
		HC06_Delay(1000);																	//wait +/- 100us just to give interrupt time to service incoming message
		if (timeout == 0) 
			return 0x02;																		//if the timeout delay is exeeded, exit with error code
	}
	if(strcmp(HC06_rx_buffer, buf) == 0)
		return 0x00;																			//success
	else
		return 0x03;																			//unknown return AT msg from HC06
}



/*********************************************************************************************
Function name   : HC06_SetName
Author 					: Grant Phillips
Date Modified   : 06/08/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Set the default Bluetooth name for the HC-06.

Special Note(s) : NONE

Parameters			: name - string that represents the new name (up to 20 characters)

Return value		: 0		-		Success
									1		-		error - more than 13 characters used for name
									2		-		Timeout error; not enough characters received for "OKsetname" message
									3		-		enough characters received, but incorrect message
*********************************************************************************************/
uint8_t HC06_SetName(char *name)
{
	uint32_t timeout = HC06_TIMEOUT_MAX;
	char buf[20];
	
	HC06_ClearRxBuffer();																//clear rx buffer
	
	if(strlen(name) > 13)																//error - name more than 20 characters
		return 0x01;
	
	sprintf(buf, "AT+NAME%s", name);
	HC06_PutStr(buf);																		//AT command for SET NAME							
	
	while(HC06_rx_counter < 9)													//wait for "OKsetname" message, i.e. 9 chars
	{
		timeout--;
		HC06_Delay(1000);																	//wait +/- 100us just to give interrupt time to service incoming message
		if (timeout == 0) 
			return 0x02;																		//if the timeout delay is exeeded, exit with error code
	}
	if(strcmp(HC06_rx_buffer, "OKsetname") == 0)
		return 0x00;																			//success
	else
		return 0x03;																			//unknown return AT msg from HC06
}



/*********************************************************************************************
Function name   : HC06_SetPin
Author 					: Grant Phillips
Date Modified   : 06/08/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Set the default Bluetooth name for the HC-06.

Special Note(s) : NONE

Parameters			: pin - string that represents the new pin number (must be 4 characters); must
												be represented by "0" - "9" characters, e.g. "1234"

Return value		: 0		-		Success
									1		-		pin less than or more than 4 characters or/and not valid 
													characters ("0" - "9")
									2		-		Timeout error; not enough characters received for "OKsetPIN" message
									3		-		enough characters received, but incorrect message
*********************************************************************************************/
uint8_t HC06_SetPin(char *pin)
{
	uint32_t timeout = HC06_TIMEOUT_MAX;
	char buf[20];
	
	HC06_ClearRxBuffer();																//clear rx buffer
	
	if((strlen(pin) < 4) || (strlen(pin) > 4))
		return 0x01;																			//error - too few or many characetrs in pin
			
	sprintf(buf, "AT+PIN%s", pin);
	HC06_PutStr(buf);																		//AT command for SET PIN						
	
	while(HC06_rx_counter < 8)													//wait for "OKsetpin" message, i.e. 8 chars
	{
		timeout--;
		HC06_Delay(1000);																	//wait +/- 100us just to give interrupt time to service incoming message
		if (timeout == 0) 
			return 0x02;																		//if the timeout delay is exeeded, exit with error code
	}
	if(strcmp(HC06_rx_buffer, "OKsetPIN") == 0)
		return 0x00;																			//success
	else
		return 0x03;																			//unknown return AT msg from HC06
}



/*********************************************************************************************
Function name   : HC06_Delay
Author 					: Grant Phillips
Date Modified   : 06/08/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)

Description			: Creates a delay using a for loop.

Special Note(s) : NONE

Parameters			: delay		-		delay value in 100us increments, i.e. delay = 1 means 100us delay

Return value		: NONE
*********************************************************************************************/
void HC06_Delay(uint32_t delay)
{
	uint32_t x;
	
	for(x=0; x<delay; x++)
	{
		//do nothing
	}
}


