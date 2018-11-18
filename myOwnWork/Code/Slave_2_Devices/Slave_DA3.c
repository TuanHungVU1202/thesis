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
bit oldstate;                                    // Old state flag

int stt1 =1, stt2 =1,stt3 = 1;
char flagReceivedAllData = 0;
char count = 0, tempReceiveData,receiveData[11];
char sendData[11];
char addressDevice1[2], addressDevice2[2], addressDevice3[2];
void RS485_send (char dat[]);                                     //send data type char to send byte of transmitting frame


// Interrupt routine
void interrupt()
{
  if(PIR1.RCIF)
  {
     while(uart1_data_ready()==0);                           //while NO data in receive register
     if(uart1_data_ready()==1)                               //if data is ready for reading
     {                                                       //see https://download.mikroe.com/documents/compilers/mikroc/pic/help/uart_library.htm#uart1_data_ready
        tempReceiveData = UART1_Read();
        if(tempReceiveData == 'S')                           //received "S", enter transmitting mode
        {
           count=0;
           receiveData[count] = tempReceiveData;
           count++;
        }
        if(tempReceiveData !='S' && tempReceiveData !='E')
        {
           receiveData[count] = tempReceiveData;
           count++;
        }
        if(tempReceiveData == 'E')                           //Receive EVERYTHING indicate by byte E, then flagReceiveAllData turns into bit 1
        {                                                    //Leads to loop while to turn On or Off relays
          receiveData[count] = tempReceiveData;
          count=0;
          flagReceivedAllData = 1;
        }
     }
  }
}
void main() {
  TRISB.B0 =0;                         //RB0 is output, shall become input if TRISB.B0=1
  TRISB.B4 =0;
  TRISB.B5 =0;

  TRISB.B3 =0;                         //Bit RS485, output

  oldstate = 0;
  UART1_Init(9600);                    //init UART1 with baudrate 9600 (capable of transferring a maximum of 9600 bits per second)
  Delay_ms(100);

  RCIE_bit = 1;                        // enable interrupt on UART1 receive
  TXIE_bit = 0;                        // disable interrupt on UART1 transmit
  PEIE_bit = 1;                        // enable peripheral interrupts
  GIE_bit = 1;                         // enable all interrupts

//Set fixed ID because setting ID configuration is not stable YET
  addressDevice1[0] = '0';             //device ID 1: 0
  addressDevice1[1] = '1';             //device ID 1: 1
  addressDevice2[0] = '0';
  addressDevice2[1] = '2';
  addressDevice3[0] = '0';
  addressDevice3[1] = '3';
  /*
  addressDevice1[0] = EEPROM_Read(0x04);
  addressDevice1[1] = EEPROM_Read(0x05);
  addressDevice2[0] = EEPROM_Read(0x08);
  addressDevice2[1] = EEPROM_Read(0x09);
  addressDevice3[0] = EEPROM_Read(0x0C);
  addressDevice3[1] = EEPROM_Read(0x0D);
  //Delay_ms(1000);
  */
  //RS485_send(send);
  Delay_ms(100);
  /*
  //Luc dau xin Master set ma vat ly cua cong tac chum 3: S 02 0000 3 1E
  if (addressDevice1[0]==0xff || addressDevice2[0]==0xff || addressDevice3[0]==0xff){
     sendData[0] = 'S';
     sendData[1] = '0';
     sendData[2] = '2';
     sendData[3] = '0';
     sendData[4] = '0';
     sendData[5] = '0';
     sendData[6] = '0';
     sendData[7] = '0';
     sendData[8] = 'D';
     sendData[9] = '3';
     sendData[10] = 'E';
     RS485_send(sendData);
  }
  */
  Delay_ms(1000);
  PORTB.RB0 =1;                       //trigger Relay1 on, LED on
  PORTB.RB4 =1;
  PORTB.RB5 =1;
  Delay_ms(500);
  PORTB.RB0 =0;                       //trigger Relay1 off, LED off
  PORTB.RB4 =0;
  PORTB.RB5 =0;
  while(1)
  {

     if(flagReceivedAllData==1){
         flagReceivedAllData = 0;                                   //after begin function, set flag back to 0 to continue receiving data
         /*
         //S12 D01 000 1 E
         if(receiveData[1] == '1' && receiveData[2] == '2'){
             if(receiveData[9] == '1'){
                 addressDevice1[0] = receiveData[4];
                 addressDevice1[1] = receiveData[5];
                 EEPROM_Write(0x04,addressDevice1[0]);
                 EEPROM_Write(0x05,addressDevice1[1]);
             }
             if(receiveData[9] == '2'){
                 addressDevice2[0] = receiveData[4];
                 addressDevice2[1] = receiveData[5];
                 EEPROM_Write(0x08,addressDevice2[0]);
                 EEPROM_Write(0x09,addressDevice2[1]);
             }
             if(receiveData[9] == '3'){
                 addressDevice3[0] = receiveData[4];
                 addressDevice3[1] = receiveData[5];
                 EEPROM_Write(0x0C,addressDevice3[0]);
                 EEPROM_Write(0x0D,addressDevice3[1]);
             }
         }
         */
         //Turn On or Off relays
         //Master sends frame to Slave Device, turn on/off Device 0x
         //S10 D0x 000 1 E
         if(receiveData[1] == '1' && receiveData[2] == '0' && receiveData[3] == 'D'){
             if(receiveData[4] == addressDevice1[0] && receiveData[5] == addressDevice1[1]){  //receiveData[4]=0 && receiveData[5]=1 => device 01
                 if(receiveData[9] == '1'){                                                   //receiveData[9]=1 => ON command
                     PORTB.RB0 =1;                                                            //trigger On pin RB0=> Relay1
                 }
                 if(receiveData[9] == '0'){
                     PORTB.RB0 =0;
                 }
             }
             if(receiveData[4] == addressDevice2[0] && receiveData[5] == addressDevice2[1]){   //receiveData[4]=0 && receiveData[5]=2
                 if(receiveData[9] == '1'){                                                    //device 02
                     PORTB.RB4 =1;                                                             //turn On Relay2
                 }
                 if(receiveData[9] == '0'){
                     PORTB.RB4 =0;
                 }
             }
             if(receiveData[4] == addressDevice3[0] && receiveData[5] == addressDevice3[1]){
                 if(receiveData[9] == '1'){
                     PORTB.RB5 =1;
                 }
                 if(receiveData[9] == '0'){
                     PORTB.RB5 =0;
                 }
             }
         }
     }

  }
}

//process data send by RS485
//see https://download.mikroe.com/documents/compilers/mikroc/pic/help/uart_library.htm#uart1_tx_idle
void RS485_send (char dat[])
{
    int i;
    PORTB.RB3 =1;                 //bit RS485 on, transmitting
    for (i=0; i<=10;i++){
    while(UART1_Tx_Idle()==0);    //while data has NOT been transmitted
    UART1_Write(dat[i]);          //transmit a byte with index i in loop for
    }
    Delay_ms(100);
    PORTB.RB3 =0;                 //bit RS485 into idle state
}