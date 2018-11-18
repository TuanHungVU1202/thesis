/*#############################################################
File Name       : ex1_HC06_setup.c
Author          : Grant Phillips
Date Modified   : 25/09/2014
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)
Tested On       : STM32F4-Discovery

Description     : Example program that uses the UsART2 module to 
									establish a serial link via a HC-06 Bluetooth
									module with another device.  This example only
									shows how to make changes to the HC-06's setup
									using the HC06_F4.c library.  The following
									functions will be demonstrated:
									HC06_Test
									HC06_SetBaud
									HC06_SetName
									HC06_SetPin
									HC06_ClearRxBuffer
									HC06_PutStr
									If the setup was successfull, the STM32F4 will
									send a continueous count as a string to another 
									device that has established a SPP connection
									with it.
									
									To show debug messages the printf statement is 
									used via the Serial Wire Viewer. See:
									http://controlsoft.nmmu.ac.za/STM32F4-Discovery-
									Board/Example-programs/Serial-Wire-Viewer-(SWV)
									
									************** SPECIAL NOTE: *****************
									The default settings for the HC-06 module is:
									Baud: 			9600
									Name:				linvor
									Pin:				1234
									If you want to change any of this, run the
									ex1_HC06_setup.c example first and make the
									necessary changes.  The rest of the example
									programs asumes that the HC-06 is running at 
									115200 baud, so at least change the buad rate
									in the setup program.  The new settings will
									be saved, even when power is removed.
									
Requirements    : * STM32F4-Discovery Board
									* Make sure the HC06_F4.c file is in the 
                    same folder than the project
              
Circuit         : * HC06 Bluetooth module is connected as follows:
										TXD			-		PA3
										RXD			-		PA2
										GND			-		GND
										VCC			-		3.3V
									* A wire link between PB3 and pin6 of the SWD
									
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
	
	uint8_t count=0;																							//8-bit integer variable to hold the count value (0-255)
	char buf[40];
	uint8_t rcv;
	
	/* Set the SysTick Interrupt to occur every 1ms) */
	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);																										//will end up in this infinite loop if there was an error with Systick_Config
	
	// ************ NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB *************
	//The HC-06 is shipped with a baud rate of 9600.  If you changed it 
	//with this setup program, you need to change HC06_Init() to this baud speed too.
	//Also keep in mind that all the sample code works with a baud of 115200.  So you can
	//either change the HC-06 baud with this setup code (see the "Set Baud" code further down)
	//or you can change the HC06_Init(115200) to HC06_Init(9600) in all the sample code.
	// ************ NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB NB *************
	HC06_Init(9200);																								//initialize the HC-06 module at 9600 baud
	
	printf("\n\nDebug messages on SWV:\n");
  Delay(1000);
	
	//Test communications with HC-06. rcv = 0 if ok
	printf("Test Communications...");
	rcv = HC06_Test();												
	if(rcv == 0)
		printf("OK\n");
	else
		printf("Err %d\n", rcv);
	
	//Set the default Bluetooth name for this STM32F4 
	//as it would be seen by other devices - MAX 13 characters
	printf("Set Name...");										
	rcv = HC06_SetName("F4_GRANT");
	if(rcv == 0)
		printf("OK\n");
	else
		printf("Err %d\n", rcv);
	
	//Set the default PIN for the HC-06 on this STM32F4 
	//which other devices will use to pair with this device
	printf("Set Pin...");
	rcv = HC06_SetPin("1234");
	if(rcv == 0)
		printf("OK\n");
	else
		printf("Err %d\n", rcv);
	
	//Set Baud rate.  If you change this, make sure next time you run this code 
	//you change the HC06_Init(115200) at the beginning to the new Baud rate
	//to effectively communicate with the module.  This new Baud rate is also
	//what it should be set to on the other device
	//
	//This section will be commented out, so if you need to change the 
	//baud rate, uncomment the next 6 lines
	
//	printf("Set Baud...");										
//	rcv = HC06_SetBaud(115200);
//	if(rcv == 0)
//		printf("OK\n");
//	else
//		printf("Err %d\n", rcv);
	
	
	//Clear the Receive buffer from any unwanted characters after the setup was done
	HC06_ClearRxBuffer();
	
	printf("Counting...");
	
	/* Main program loop */
  while (1)
  {
		sprintf(buf, "%2X\n", count); 															//print a integer value (in HEX) and newline to the buf string
		HC06_PutStr(buf);																						//write this string via the HC-06 Bluetooth module as a serial string
		
		count = count + 1;
    Delay(250);
  }	
}


/*
Retarget the C library printf function to the SWO Viewer.
Overwrites int fputc(int ch, FILE *f) function used by printf.
*/								
int fputc(int ch, FILE *f)
{
	return(ITM_SendChar(ch));
}

