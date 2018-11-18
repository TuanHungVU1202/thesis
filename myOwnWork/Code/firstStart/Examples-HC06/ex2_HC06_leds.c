/*#############################################################
File Name       : ex2_HC06_leds.c
Author          : Grant Phillips
Date Modified   : 25/09/2014
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)
Tested On       : STM32F4-Discovery

Description     : Example program that uses the USART2 module to 
									establish a serial link via a HC-06 Bluetooth
									module with another device.  If the STM32F4
									receives the following character:
									"A"		-	ORANGE LED (LD3) ON
									"B"		-	ORANGE LED (LD3) OFF
									"C"		-	RED LED (LD5) ON
									"D"		-	RED LED (LD5) OFF
									"E"		- BLUE LED (LD6) ON
									"F"		-	BLUE LED (LD6) OFF
									"G"		-	GREEN LED (LD4) ON
									"H"		-	GREEN LED (LD4) OFF

Requirements    : * STM32F4-Discovery Board
									* Make sure the HC06_F4.c file is in the 
                    same folder than the project
              
Circuit         : * 4 LEDs connected to PD15 - PD12 (LD6 - LD4
                    ON STM32F4-DISCOVERY BOARD)
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

/* #global variables -----------------------------------------*/



int main(void)
{
	RCC_ClocksTypeDef 			RCC_Clocks;														//structure used for setting up the SysTick Interrupt
	
	/* Set the SysTick Interrupt to occur every 1ms) */
	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);																										//will end up in this infinite loop if there was an error with Systick_Config
	
	HC06_Init(115200);																						//initialize the HC-06 module at 115200 baud
	InitLEDs();																									  //initialize the USER LEDs for this application
	
	HC06_ClearRxBuffer();																					//clear the rx buffer after initialization
		
	/* Main program loop */
  while (1)
  {
		if(new_HC06_msg)															//check if there is a new message
		{
			if(strcmp(HC06_msg, "A") == 0)							//ORANGE LED (LD3) ON
				GPIO_WriteBit (GPIOD, GPIO_Pin_13, Bit_SET);
			else if(strcmp(HC06_msg, "B") == 0)					//ORANGE LED (LD3) OFF
				GPIO_WriteBit (GPIOD, GPIO_Pin_13, Bit_RESET);
			else if(strcmp(HC06_msg, "C") == 0)					//RED LED (LD5) ON
				GPIO_WriteBit (GPIOD, GPIO_Pin_14, Bit_SET);
			else if(strcmp(HC06_msg, "D") == 0)					//RED LED (LD5) OFF
				GPIO_WriteBit (GPIOD, GPIO_Pin_14, Bit_RESET);
			else if(strcmp(HC06_msg, "E") == 0)					//BLUE LED (LD6) ON
				GPIO_WriteBit (GPIOD, GPIO_Pin_15, Bit_SET);
			else if(strcmp(HC06_msg, "F") == 0)					//BLUE LED (LD6) OFF
				GPIO_WriteBit (GPIOD, GPIO_Pin_15, Bit_RESET);
			else if(strcmp(HC06_msg, "G") == 0)					//GREEN LED (LD4) ON
				GPIO_WriteBit (GPIOD, GPIO_Pin_12, Bit_SET);
			else if(strcmp(HC06_msg, "H") == 0)					//GREEN LED (LD4) OFF
				GPIO_WriteBit (GPIOD, GPIO_Pin_12, Bit_RESET);
			else
				HC06_PutStr("Incorrect command\n");				//Reply incorrect command
				
			new_HC06_msg=0;															//clear message flag
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




