#line 1 "C:/Users/HP/Google Drive (1450231@hcmut.edu.vn)/BK/Thesis/myOwnWork/Code/Slave_2_Buttons/Slave_CA3.c"
#line 21 "C:/Users/HP/Google Drive (1450231@hcmut.edu.vn)/BK/Thesis/myOwnWork/Code/Slave_2_Buttons/Slave_CA3.c"
bit oldstate;
int stt1 =1, stt2 =1,stt3 = 1;
int busy = 0;
char flagReceivedAllData = 0;
char count = 0, tempReceiveData,receiveData[11];
char sendData[11];
char addressButton1[2], addressButton2[2];
char addressDevice1[2], addressDevice2[2];
void RS485_send (char dat[]);
void checkstt (int stt);


void interrupt()
{
 if(PIR1.RCIF)
 {
 while(uart1_data_ready()==0);
 if(uart1_data_ready()==1)
 {
 tempReceiveData = UART1_Read();
 if(tempReceiveData == 'S')
 {
 busy = 1;
 count = 0;
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
 TRISB.B0 =1;
 TRISB.B4 =1;
 TRISB.B5 =1;

 TRISB.B3 =0;

 oldstate = 0;
 UART1_Init(9600);
 Delay_ms(100);

 RCIE_bit = 1;
 TXIE_bit = 0;
 PEIE_bit = 1;
 GIE_bit = 1;
#line 83 "C:/Users/HP/Google Drive (1450231@hcmut.edu.vn)/BK/Thesis/myOwnWork/Code/Slave_2_Buttons/Slave_CA3.c"
 addressButton1[0] = '0';
 addressButton1[1] = '6';
 addressDevice1[0] = '0';
 addressDevice1[1] = '6';

 addressButton2[0] = '0';
 addressButton2[1] = '7';
 addressDevice2[0] = '0';
 addressDevice2[1] = '7';
#line 111 "C:/Users/HP/Google Drive (1450231@hcmut.edu.vn)/BK/Thesis/myOwnWork/Code/Slave_2_Buttons/Slave_CA3.c"
 Delay_ms(100);
#line 129 "C:/Users/HP/Google Drive (1450231@hcmut.edu.vn)/BK/Thesis/myOwnWork/Code/Slave_2_Buttons/Slave_CA3.c"
 while(1)
 {
#line 182 "C:/Users/HP/Google Drive (1450231@hcmut.edu.vn)/BK/Thesis/myOwnWork/Code/Slave_2_Buttons/Slave_CA3.c"
 if (Button(&PORTB, 0, 1, 1)) {
 oldstate = 1;
 }
 if (oldstate && Button(&PORTB, 0, 1, 0)) {
 Delay_ms(100);
 if (oldstate && Button(&PORTB, 0, 1, 0))
 {
#line 193 "C:/Users/HP/Google Drive (1450231@hcmut.edu.vn)/BK/Thesis/myOwnWork/Code/Slave_2_Buttons/Slave_CA3.c"
 sendData[0] = 'S';
 sendData[1] = '0';
 sendData[2] = '0';
 sendData[3] = 'B';
 sendData[4] = '0';
 sendData[5] = '6';


 sendData[6] = 'D';
 sendData[7] = '0';
 sendData[8] = '6';


 sendData[9] = '0';
 sendData[10] = 'E';
 checkstt(stt1);
 stt1++;
 while(busy == 1){
 ;
 }
 Delay_ms(10);
 RS485_send(sendData);
 Delay_ms(100);

 oldstate = 0;
 Delay_ms(500);
 }
 }


 if (Button(&PORTB, 4, 1, 1)) {
 oldstate = 1;
 }
 if (oldstate && Button(&PORTB, 4, 1, 0)) {
 Delay_ms(100);
 if (oldstate && Button(&PORTB, 4, 1, 0))
 {
 sendData[0] = 'S';
 sendData[1] = '0';
 sendData[2] = '0';
 sendData[3] = 'B';
#line 238 "C:/Users/HP/Google Drive (1450231@hcmut.edu.vn)/BK/Thesis/myOwnWork/Code/Slave_2_Buttons/Slave_CA3.c"
 sendData[4] = '0';
 sendData[5] = '7';
 sendData[6] = 'D';
#line 245 "C:/Users/HP/Google Drive (1450231@hcmut.edu.vn)/BK/Thesis/myOwnWork/Code/Slave_2_Buttons/Slave_CA3.c"
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

 oldstate = 0;
 Delay_ms(500);
 }
 }
 }
}


void RS485_send (char dat[])
{
 int i;
 PORTB.RB3 =1;
 Delay_ms(100);
 for (i=0; i<=10;i++){
 while(UART1_Tx_Idle()==0);
 UART1_Write(dat[i]);
 }
 Delay_ms(100);
 PORTB.RB3 =0;
}
#line 285 "C:/Users/HP/Google Drive (1450231@hcmut.edu.vn)/BK/Thesis/myOwnWork/Code/Slave_2_Buttons/Slave_CA3.c"
void checkstt (int stt)
{
 if (stt % 2 == 0)
 sendData[9] = '0';
 else
 sendData[9] = '1';
}