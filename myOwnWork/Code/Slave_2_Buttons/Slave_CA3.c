/*
Cong tac 3 nut nhan
Dia chi nut nhan 1 BIT RB0:
Dia chi Cong tac luu vao o nho 0x02,0x03
O cam ma Cong tac dieu khien luu vao 0x04,0x05

Dia chi nut nhan 2 BIT RB4:
Dia chi Cong tac luu vao o nho 0x06,0x07
O cam ma Cong tac dieu khien luu vao 0x08,0x09

Dia chi nut nhan 3 BIT RB5:
Dia chi Cong tac luu vao o nho 0x06,0x07
O cam ma Cong tac dieu khien luu vao 0x08,0x09

VD: Slave 3 nut xin cap dia chi: S02 00 00 3 1E
    Master nhan va cap 3 dia chi:
    S 12 01 06 1 1E :01 la cong tac, 06 la o cam no dieu khien
    S 12 02 07 2 1E
    S 12 03 08 3 1E
*/
bit oldstate;                                                    //Old state flag
int stt1 =1, stt2 =1,stt3 = 1;                                   //later pass sttX into function checkstt(sttX)
int busy = 0;
char flagReceivedAllData = 0;
char count = 0, tempReceiveData,receiveData[11];
char sendData[11];
char addressButton1[2], addressButton2[2];
char addressDevice1[2], addressDevice2[2];
void RS485_send (char dat[]);                                    //later pass sendData[] to char dat[]
void checkstt (int stt);

// Interrupt routine
void interrupt()
{
  if(PIR1.RCIF)
  {
     while(uart1_data_ready()==0);                             //while NO data in receive register
     if(uart1_data_ready()==1)                                 //if data is ready for reading
     {
        tempReceiveData = UART1_Read();                        //see https://download.mikroe.com/documents/compilers/mikroc/pic/help/uart_library.htm#uart1_data_ready
        if(tempReceiveData == 'S')                             //received "S", enter transmitting mode
        {
           busy = 1;                                           //then UART will be busy
           count = 0;                                          //use var count to index received Data
           receiveData[count] = tempReceiveData;
           count++;
        }
        if(tempReceiveData !='S' && tempReceiveData !='E')
        {
           receiveData[count] = tempReceiveData;
           count++;
        }
        if(tempReceiveData == 'E')
        {
          receiveData[count] = tempReceiveData;
          count=0;
          flagReceivedAllData = 1;
          busy = 0;
        }
     }
  }
}
void main() {
  TRISB.B0 =1;                         //bit Button 1, input. Shall become output if value = 0
  TRISB.B4 =1;                         //bit Button 2, input
  TRISB.B5 =1;                         //bit Button 3, intput

  TRISB.B3 =0;                         //Bit RS485, output

  oldstate = 0;
  UART1_Init(9600);                    //init UART1 with baudrate 9600 (capable of transferring a maximum of 9600 bits per second)
  Delay_ms(100);

  RCIE_bit = 1;                        // enable interrupt on UART1 receive
  TXIE_bit = 0;                        // disable interrupt on UART1 transmit
  PEIE_bit = 1;                        // enable peripheral interrupts
  GIE_bit = 1;                         // enable all interrupts

/*
Set fixed ID because setting ID configuration is not stable YET
id of Button 1 and Device 1 comes together to control D01 with B01
*/
  addressButton1[0] = '0';             //ID Button 1: 0
  addressButton1[1] = '6';             //ID Button 1: 1
  addressDevice1[0] = '0';             //ID Device 1: 0
  addressDevice1[1] = '6';             //ID Device 1: 1

  addressButton2[0] = '0';
  addressButton2[1] = '7';
  addressDevice2[0] = '0';
  addressDevice2[1] = '7';

  /*
  addressButton1[0] = EEPROM_Read(0x02);
  addressButton1[1] = EEPROM_Read(0x03);
  addressDevice1[0] = EEPROM_Read(0x04);
  addressDevice1[1] = EEPROM_Read(0x05);
  
  addressButton2[0] = EEPROM_Read(0x06);
  addressButton2[1] = EEPROM_Read(0x07);
  addressDevice2[0] = EEPROM_Read(0x08);
  addressDevice2[1] = EEPROM_Read(0x09);
  
  addressButton3[0] = EEPROM_Read(0x0A);
  addressButton3[1] = EEPROM_Read(0x0B);
  addressDevice3[0] = EEPROM_Read(0x0C);
  addressDevice3[1] = EEPROM_Read(0x0D);
  //Delay_ms(1000);
  */
  //RS485_send(send);
  Delay_ms(100);
  //Luc dau xin Master set ma vat ly cua cong tac chum 3: S 02 0000 3 1E
  /*
  if (addressButton1[0]==0xff || addressButton2[0]==0xff || addressButton3[0]==0xff){
     sendData[0] = 'S';
     sendData[1] = '0';
     sendData[2] = '2';
     sendData[3] = '0';
     sendData[4] = '0';
     sendData[5] = '0';
     sendData[6] = '0';
     sendData[7] = '0';
     sendData[8] = 'B';
     sendData[9] = '3';
     sendData[10] = 'E';
     RS485_send(sendData);
  }
  */
  while(1)
  {
     /*
     if(flagReceivedAllData==1){
         flagReceivedAllData = 0;
         //S12 B01 D01 1 E

         if(receiveData[2] == '2'){
             if(receiveData[9] == '1'){
                 addressButton1[0] = receiveData[4];
                 addressButton1[1] = receiveData[5];
                 addressDevice1[0] = receiveData[7];
                 addressDevice1[1] = receiveData[8];
                 EEPROM_Write(0x02,addressButton1[0]);
                 EEPROM_Write(0x03,addressButton1[1]);
                 EEPROM_Write(0x04,addressDevice1[0]);
                 EEPROM_Write(0x05,addressDevice1[1]);
             }
             if(receiveData[9] == '2'){
                 addressButton2[0] = receiveData[4];
                 addressButton2[1] = receiveData[5];
                 addressDevice2[0] = receiveData[7];
                 addressDevice2[1] = receiveData[8];
                 EEPROM_Write(0x06,addressButton2[0]);
                 EEPROM_Write(0x07,addressButton2[1]);
                 EEPROM_Write(0x08,addressDevice2[0]);
                 EEPROM_Write(0x09,addressDevice2[1]);
             }
             if(receiveData[9] == '3'){
                 addressButton3[0] = receiveData[4];
                 addressButton3[1] = receiveData[5];
                 addressDevice3[0] = receiveData[7];
                 addressDevice3[1] = receiveData[8];
                 EEPROM_Write(0x0A,addressButton3[0]);
                 EEPROM_Write(0x0B,addressButton3[1]);
                 EEPROM_Write(0x0C,addressDevice3[0]);
                 EEPROM_Write(0x0D,addressDevice3[1]);
             }
         }

     }
     */
     
/********************************
     Procedures below dealing with controlling devices with Buttons on Slave Buttons
     see https://download.mikroe.com/documents/compilers/mikroc/pic/help/button_library.htm
     Button Lib helps avoiding deboucing phenomenom
     Syntax Button(port, pin, time, Active_state)
     +Port is location (port B)
     +Pin number (RB0...RB7, etc)
     +Time is debounce period (ms)
     +Active_state either 0 or 1 means active at logical 0 or 1, respectively
*********************************/
    if (Button(&PORTB, 0, 1, 1)) {               // Detect logical 1 => ON device
      oldstate = 1;                              // Update flag
    }
    if (oldstate && Button(&PORTB, 0, 1, 0)) {   // Detect one-to-zero transition (oldstate=1) from previous if
      Delay_ms(100);                             // oldstate=1 && portB turns to 0
      if (oldstate && Button(&PORTB, 0, 1, 0))   // => OFF device using below frame
      {
      /*
      sendData to Master frame S00 B01 D01 0 E
      to turn OFF Device D01 by Button B01
      */
         sendData[0] = 'S';
         sendData[1] = '0';
         sendData[2] = '0';
         sendData[3] = 'B';
         sendData[4] = '0';
         sendData[5] = '6';
         //sendData[4] = addressButton1[0];
         //sendData[5] = addressButton1[1];
         sendData[6] = 'D';
         sendData[7] = '0';
         sendData[8] = '6';
         //sendData[7] = addressDevice1[0];
         //sendData[8] = addressDevice1[1];
         sendData[9] = '0';                          //bit turn OFF
         sendData[10] = 'E';                         //end frame
         checkstt(stt1);
         stt1++;                                     //increase stt1, init stt1=1
         while(busy == 1){
            ;
         }
         Delay_ms(10);
         RS485_send(sendData);
         Delay_ms(100);
         //RS485_send(sendData);
         oldstate = 0;                           // Update flag
         Delay_ms(500);
      }
    }


    if (Button(&PORTB, 4, 1, 1)) {               // Detect logical one => ON device
      oldstate = 1;                              // Update flag
    }
    if (oldstate && Button(&PORTB, 4, 1, 0)) {   // Detect one-to-zero transition => OFF device using below frame
      Delay_ms(100);
      if (oldstate && Button(&PORTB, 4, 1, 0))
      {
         sendData[0] = 'S';
         sendData[1] = '0';
         sendData[2] = '0';
         sendData[3] = 'B';
         /*
         sendData[4] = addressButton2[0];
         sendData[5] = addressButton2[1];
         */
         sendData[4] = '0';
         sendData[5] = '7';
         sendData[6] = 'D';
         /*
         sendData[7] = addressDevice2[0];
         sendData[8] = addressDevice2[1];
         */
         sendData[7] = '0';
         sendData[8] = '7';
         sendData[9] = '0';
         sendData[10] = 'E';
         checkstt(stt2);
         stt2++;
         while(busy == 1){
            ;
         }
         Delay_ms(10);
         RS485_send(sendData);
         Delay_ms(100);  
         //RS485_send(sendData);
         oldstate = 0;                           // Update flag
         Delay_ms(500);
      }
    }
  }
}
//process data send by RS485
//see https://download.mikroe.com/documents/compilers/mikroc/pic/help/uart_library.htm#uart1_tx_idle
void RS485_send (char dat[])
{
    int i;
    PORTB.RB3 =1;                        //bit RS485 on, transmitting
    Delay_ms(100);
    for (i=0; i<=10;i++){
    while(UART1_Tx_Idle()==0);           //while data has NOT been transmitted
    UART1_Write(dat[i]);                 //sendData[] passed into function and 10 char of command frame is transmitted
    }
    Delay_ms(100);
    PORTB.RB3 =0;                        //bit RS485 into idle state
}

/**************
  check stt Device if it turn ON or OFF
  sendData[9] is bit before end frame bit "E"
  0 is OFF device
  1 is ON device
********************/
void checkstt (int stt)
{
    if (stt % 2 == 0)       //if stt chia het cho 2 => even number
    sendData[9] = '0';      //then turn OFF device (press button Twice)
    else
    sendData[9] = '1';      //else turn ON device (press button Once)
}