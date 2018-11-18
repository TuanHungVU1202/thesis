/*#############################################################
File Name       : ex3_HC06_leds_sw_analog.c
Author          : Grant Phillips
Date Modified   : 25/09/2014
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)
Tested On       : STM32F4-Discovery

Description     : Example program that uses the USART2 module to 
									establish a serial link via a HC-06 Bluetooth
									module with another device.  The purpose of the 
									STM32F4 is to control 4 LEDs, read a pushbutton, 
									and read an analog input.  
									The message protocol is as follows:
              
                   byte (HEX) byte (HEX) byte(HEX)  byte(HEX) byte(HEX)
                  ------------------------------------------------------
                  | COMMAND  |   DATA2  |   DATA1  |  DATA0  |    LF   |
                  ------------------------------------------------------
                  
									(from other device to this STM32F4)
                  COMMAND  DESCRIPTION
                  =======  ===========
                  00       Set the LEDs to the value of DATA2.  
                           DATA1 and DATA0 is not used
                  01       Set the LED flash rate to the value
                           of DATA2.  DATA1 and DATA0 is not used.
                  02       Flash the LEDs.  DATA2, DATA1 and DATA0 are 
                           not used.
                  03       Stop flashing the LEDs.  DATA2, DATA1 and 
                           DATA0 are not used.
									04       Request the value of the pushbutton. 
                           DATA2, DATA1 and DATA0 are not used.
									06       Request the value of the analog input. 
                           DATA2, DATA1 and DATA0 are not used.
									08       Request the value of BOTH the pushbutton and
													 the analog input.  DATA2, DATA1 and DATA0 are 
													 not used.
													 
									(from this STM32F4 to other device)
                  COMMAND  DESCRIPTION
                  =======  ===========
                  05       Reply from the STM32F4 with pushbutton
													 value. DATA2 contains the value. DATA1 
                           and DATA0 are not used.
                  07       Reply from the STM32F4 with analog value. 
                           DATA2 contains the high byte of the 
                           12-bit value and DATA1 the low byte. 
													 DATA0 is not used.
									09       Reply from the STM32F4 with BOTH the 
													 pushbutton value and the analog value. 
                           DATA2 contains the pushbutton value. DATA1 
													 contains the high byte of the 12-bit value 
													 and DATA0 the low byte. 						

                  E.g.
                  If the pushbutton is on, and the other device sends
                  the command: "04", then the reply from the STM32F4 
									will be:  "0501"; when it was off it would be "0500"
                  
                  E.g.
                  If the analog value is at maximum (4095 or
                  0x0FFF), and the other device sends the command: 
                  "06", then the reply from the STM32F4 will 
                  be:  "0703FF"
									
									E.g.
									If the pushbutton is on and the analog value is at 
									maximum, and the other device sends the command:
									"08" to get BOTH the pushbutton value and analog value
									in one reply, then the reply from the STM32F4 will
									be: "09010FFF"
                  
                  Note that all values must be entered in 
                  CAPITAL.

Requirements    : * STM32F4-Discovery Board
									* Make sure the HC06_F4.c file is in the 
                    same folder than the project
              
Circuit         : * 4 LEDs connected to PD15 - PD12 (LD6 - LD4
                    ON STM32F4-DISCOVERY BOARD)
									* a pushbutton connected to PA0 (USER BUTTON
										STM32F4-DISCOVERY BOARD)
									* a potentiometer (POT) connected to PC1 
                    (ADC1_IN11 - Analog1 Channel 11)
									* HC06 Bluetooth module is connected as follows:
										TXD			-		PA3
										RXD			-		PA2
										GND			-		GND
										VCC			-		3.3V
									
See the STM32F4-Discovery User Manual (UM1472) for the block 
diagram of the STM32F407VGT6 processor (p12), a summary of
the GPIO connections (p20-29) and the schematic diagram (p31-36)

##############################################################*/

/* #includes -------------------------------------------------*/
#include "STM32F4_Defaults.c"																		//library for the defaults of a blank project in Keil uVision
#include "HC06_F4.c"																						//for the HC06 functions on the STM32F4-Discovery

/* #defines --------------------------------------------------*/

/* #function prototypes --------------------------------------*/
void InitLEDs(void);																					  //prototype for the user function to initialize the USER LEDs
void WriteLEDs(unsigned char value);														//function to write a value (0 - 15) to the 4 LEDs
unsigned char HexStToByte(char *St3);														//funtion to convert a HEX string to a byte value
void IntToHexStr(unsigned int data,char *St5);									//function to convert a 16-bit value to a HEX string
void CharToHexStr(unsigned int data,char *St3);									//function to convert a 8-bit value to a HEX string
void InitAtoD(void);																						//prototype for the user function to initialize the ADC
void InitPB(void);																							//prototype for the user function to initialize the Pushbutton

/* #global variables -----------------------------------------*/


	
int main(void)
{
	RCC_ClocksTypeDef 			RCC_Clocks;														//structure used for setting up the SysTick Interrupt
	
	unsigned char COMMAND, SETTINGS;
	unsigned char ledvalue=0X00;  		//default LEDs value
	unsigned char leddelay=100;   		//default flash delay
	unsigned char ledflash=0;     		//flag 0 = no flash, 1 = flash
	unsigned char ledtoggle=0;
	unsigned char flashcounter=0;			//counter to keep track of how many ms have passed
	char sendbuf[20];									//buffer string used for sending a message
	char buf[20];											//buffer string used for HEX and INT conversions
	char buf2[20];										//buffer string used for HEX and INT conversions
	unsigned int ADC_val;							//variable for holding the 12-bit ADC result
	unsigned char pb_val;         		//8-bit variable to hold the switches value
	
	/* Set the SysTick Interrupt to occur every 1ms) */
	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);																										//will end up in this infinite loop if there was an error with Systick_Config
	
	HC06_Init(115200);																						//initialize the HC-06 module at 115200 baud
	InitLEDs();																									  //initialize the USER LEDs for this application
	InitAtoD();																										//initialize the ADC for this application
	InitPB();																											//initialize the USER button for this application
	
	HC06_ClearRxBuffer();																					//clear the rx buffer after initialization
	
	/* Main program loop */
  while (1)
  {
		/* Get ADC1 data */
		ADC_SoftwareStartConv(ADC1);															//start ADC1 conversion
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));						//wait until conversion finnished
		ADC_val = ADC_GetConversionValue(ADC1); 									//read converted data
		
		/* Read the pushbutton value */
    pb_val = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0); 
		
		if (ledflash == 1)
		{
			Delay(1);														//delay 1 ms
			flashcounter++;
			if(flashcounter == leddelay)				
			{
				flashcounter=0;
				if(ledtoggle == 0)
				{
					ledtoggle=1;
					WriteLEDs(0);										//clear LEDs
				}
				else
				{
					ledtoggle=0;
					WriteLEDs(ledvalue);						//write ledvalue to the LEDs
				}	
			}
		}
		if(new_HC06_msg == 1)																				//is there a new message?
		{
			/* HC06_msg[0..1] contains the COMMAND */
			buf[0] = HC06_msg[0];
			buf[1] = HC06_msg[1];
			buf[2] = '\0';																						//buf contains COMMAND HEX string
			COMMAND = HexStToByte(buf);																//convert buf to a value
			if (COMMAND == 0)																					//if COMMAND = 0 (SET_LEDS)
			{
				/* HC06_msg[2..3] contains the SETTINGS */
				buf[0] = HC06_msg[2];
				buf[1] = HC06_msg[3];
				buf[2] = '\0';																					//buf contains SETTINGS HEX string
				SETTINGS = HexStToByte(buf);														//convert buf to a value
				ledvalue = SETTINGS;
				WriteLEDs(ledvalue);																		//write the SETTINGS value to the LEDs
			}
			else if (COMMAND == 1)																		//if COMMAND = 1 (SET_FLASH_DELAY)
			{
				buf[0] = HC06_msg[2];
				buf[1] = HC06_msg[3];
				buf[2] = '\0';
				SETTINGS = HexStToByte(buf);
				leddelay = SETTINGS;
			}
			else if (COMMAND == 2)																		//if COMMAND = 2 (START_FLASH)
			{
				ledflash=1;
				flashcounter=0;
			}
			else if (COMMAND == 3)																		//if COMMAND = 3 (STOP_FLASH)
			{
				ledflash=0;
			}
			else if (COMMAND == 4)																		//if COMMAND = 4 (REQUEST_PB)
			{
				CharToHexStr(pb_val, buf);         
				//we could have used printf("05%X%c", sw_val, 10), but according to our
				//protocol all HEX values must have a leading 0 if it is less than 2 characters wide.
				//CharToHexStr() creates a string from a byte value and automatically adds a leading
				//0 where necessary
				sprintf(sendbuf, "05%s\n", buf); 				//write the cmd 05, the pb_val (with leading
																								//0 if necessary), and the NL character to a string buf
				HC06_PutStr(sendbuf);
			}
			else if (COMMAND == 6)																		//if COMMAND = 6 (REQUEST_ANALOG)
			{
				IntToHexStr(ADC_val, buf);          		//same as CharToHexStr, but writes a integer (16-bits)
																								//to string with leading 0's where required
				sprintf(sendbuf, "07%s\n", buf);  		  //write the cmd 07, the ADC_Val (with leading
																								//0 if necessary), and the NL character to a string buf
				HC06_PutStr(sendbuf);
			}
			else if (COMMAND == 8)																		//if COMMAND = 6 (REQUEST_PB_ANALOG)
			{
				CharToHexStr(pb_val, buf);         
				IntToHexStr(ADC_val, buf2);          	
				sprintf(sendbuf, "09%s%s\n", buf, buf2);  		//write the cmd 09, the pb-val, the ADC_Val (with leading
																											//0 if necessary), and the NL character to a string buf
				HC06_PutStr(sendbuf);
			}
			else
			{
				//incorrect command
			}
			new_HC06_msg = 0;																					//clear the flag to indicate that the new message was serviced
		}
  }	
}



void InitLEDs(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;										//structure used for setting up a GPIO port
	
  /* GPIOD Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);           
  
  /* Configure PD15 - PD12 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;               
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                    
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;                     
  GPIO_Init(GPIOD, &GPIO_InitStructure);                                    
}



void WriteLEDs(unsigned char value)
{
	/* use bit masking on input value to write to the LEDs */
	if((value & 0x01) > 0)														//test bit0
		GPIO_WriteBit (GPIOD, GPIO_Pin_12, Bit_SET);		//GREEN LED (LD4) ON
	else 				
		GPIO_WriteBit (GPIOD, GPIO_Pin_12, Bit_RESET);	//GREEN LED (LD4) OFF
	
	if((value & 0x02) > 0)														//test bit1
		GPIO_WriteBit (GPIOD, GPIO_Pin_13, Bit_SET);		//ORANGE LED (LD3) ON
	else																						
		GPIO_WriteBit (GPIOD, GPIO_Pin_13, Bit_RESET);	//ORANGE LED (LD3) OFF
	
	if((value & 0x04) > 0)														//test bit2
		GPIO_WriteBit (GPIOD, GPIO_Pin_14, Bit_SET);		//RED LED (LD5) ON
	else 					
		GPIO_WriteBit (GPIOD, GPIO_Pin_14, Bit_RESET);	//RED LED (LD5) OFF
	
	
	if((value & 0x08) > 0)														//test bit3
		GPIO_WriteBit (GPIOD, GPIO_Pin_15, Bit_SET);		//BLUE LED (LD6) ON
	else 					
		GPIO_WriteBit (GPIOD, GPIO_Pin_15, Bit_RESET);	//BLUE LED (LD6) OFF
}



void InitPB(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;										//structure used for setting up a GPIO port
	
  /* GPIOA Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	/* Configure PA0 in input mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;									
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;							
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}



void InitAtoD(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
  ADC_InitTypeDef       	ADC_InitStructure;										//structure used for setting up the ADC
	
	/* Configure the ADC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);						//ADC1 is connected the APB2 peripheral bus 
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN,ENABLE);						//Clock for the ADC port!! Do not forget about this one
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);					//enable the AHB1 bus to use GPIOC
			
  /* Configure ADC1 Channel11 (PC1) as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;											//The channel 11 is connected to PC1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN; 									//The PC1 pin is configured in analog mode
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 							//output not pulled up or down
	GPIO_Init(GPIOC,&GPIO_InitStructure);
  
	/* ADC structure configuration */
	ADC_DeInit();
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;											//data converted will be shifted to right
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;											//Input voltage is converted into a 12bit number giving a maximum value of 4096
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 													//the conversion is continuous, the input data is converted more than once
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;				//conversion is synchronous with TIM1 and CC1 ???
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//no trigger for conversion
	ADC_InitStructure.ADC_NbrOfConversion = 1;																	//I think this one is clear :p
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;																//The scan is configured in one channel
	ADC_Init(ADC1,&ADC_InitStructure);																					//Initialize ADC
	
	/* Enable ADC conversion */
	ADC_Cmd(ADC1,ENABLE);
	
	/* Select the channel to be read from */
	ADC_RegularChannelConfig(ADC1,ADC_Channel_11, 1, ADC_SampleTime_144Cycles);		
}



//////////////////////////////////////////////////////////////////
//                                                              //
// Function name   : HexStrToByte                               //
//                                                              //
// Original auther : Riaan Ehlers                               //
// Date Modufied   : 04/04/2012                                 //
// Description     : Converts a 3 byte string to a byte         //
//                   eg. "E0" -> 224=0xE0                       //
//                       "12" ->  18=0x12                       //
// Note            : The string must have a string length of 3. //
//                   The string must be 2 uppercase ascii       //
//                   characters followed by an optional null    //
//                   terminator.                                //
//                   Values like "3f" is not seen as hex.       //
//                                                              //
// Compiler        : MPLAB C30 C Copiler ver 3.24               //
//////////////////////////////////////////////////////////////////

unsigned char HexStToByte(char *St3) //eg. char St3[3]="3F" -> return 0x3F
{
 unsigned char Ans=0;

//Higher Nibble
 if(St3[0]<='9') //'0'..'9'
  Ans=(St3[0]-'0')*0x10;
 else
  Ans=(St3[0]-0x37)*0x10; //  Ans+=(St[i]-'A'+0x0A)*0x10;

//Higher Lower
 if(St3[1]<='9') //'0'..'9'
  Ans+=(St3[1]-'0');
 else
  Ans+=(St3[1]-0x37); //  Ans+=(St[i]-'A'+0x0A)*0x01;

 return(Ans);
}



//////////////////////////////////////////////////////////////////
//                                                              //
// Function name   : IntToHexStr                                 //
//                                                              //
// Original auther : Morne Odendaal & Riaan Ehlers              //
// Date Modufied   : 30/06/2004                                 //
// Description     : Converts a 16bit unsigned integer          //
//                   value to a string.                         //
//                   eg. 57515=0xE0AB -> "E0AB"                 //
//                   Min size of St5=5 bytes (char St5[5])      //
//                                                              //
// Note            : rather use printf("%x") for CCS compiler   //
// Compiler        : MPLAB C18 C Copiler ver 2.20               //
//////////////////////////////////////////////////////////////////

void IntToHexStr(unsigned int data,char *St5)
{
 char nibble;
 unsigned int place,x;
 char StPtr=0;

 x=16;
 for (place = 0xF000;place >= 0x000F;place=place>>4)
 {           
  x=x - 0x4;
  nibble = ((data & place) >> x) + 0x30; //0x30='0'
  if (nibble > 0x39)                     //if(nibble>'9')
  {
   nibble = nibble + 0x07;               //Add ascii offset. There are 7 characters between '9' and 'A'
  }//if ((nibble != 0) && (place == 0xF000)) 

  St5[StPtr]=nibble;
  StPtr++;
 }//for
 St5[4]='\0'; //Terminate string
}



//////////////////////////////////////////////////////////////////
//                                                              //
// Function name   : CharToHexStr                               //
//                                                              //
// Tested by       : Riaan Ehlers                               //
// Date Modified   : 18/06/2012                                 //
// Comment         : Tested with MPLAB C18 C Compiler ver 3.36  //
//                                                              //
// Original Auther : Riaan Ehlers                               //
// Date Modified   : 28/02/2012                                 //
// Description     : Convert the lower byte of a 16bit unsigned //
//                   integer to a string.                       //
//                   eg. 249=0xF9 -> "F9" (Good for Bin to text)//
//                       153=0x99 -> "99" (Good for BCD to text)//
//                   Min size of St3=3 bytes (char St3[3])      //
// Compiler        : MPLAB C30 C Compiler ver 3.24              //
//                                                              //
//////////////////////////////////////////////////////////////////

void CharToHexStr(unsigned int data,char *St3) //char St3[3];
{
 int nibble;

 nibble = ((data & 0xF0) >> 4) + '0';
 if (nibble > '9')
 {
  nibble = nibble + 0x07;               //Add ascii offset. There are 7 characters between '9' and 'A'
 } 
 St3[0]=nibble; //'0'..'F'

 nibble = (data & 0x0F) + '0';
 if (nibble > '9')
 {
  nibble = nibble + 0x07;               //Add ascii offset. There are 7 characters between '9' and 'A'
 } 
 St3[1]=nibble; //'0'..'F'

 St3[2]='\0'; //Terminate string
}

