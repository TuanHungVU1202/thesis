
_interrupt:
	MOVWF      R15+0
	SWAPF      STATUS+0, 0
	CLRF       STATUS+0
	MOVWF      ___saveSTATUS+0
	MOVF       PCLATH+0, 0
	MOVWF      ___savePCLATH+0
	CLRF       PCLATH+0

;Slave_1_Button.c,34 :: 		void interrupt()
;Slave_1_Button.c,36 :: 		if(PIR1.RCIF)
	BTFSS      PIR1+0, 5
	GOTO       L_interrupt0
;Slave_1_Button.c,38 :: 		while(uart1_data_ready()==0);                             //while NO data in receive register
L_interrupt1:
	CALL       _UART1_Data_Ready+0
	MOVF       R0+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt2
	GOTO       L_interrupt1
L_interrupt2:
;Slave_1_Button.c,39 :: 		if(uart1_data_ready()==1)                                 //if data is ready for reading
	CALL       _UART1_Data_Ready+0
	MOVF       R0+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt3
;Slave_1_Button.c,41 :: 		tempReceiveData = UART1_Read();                        //see https://download.mikroe.com/documents/compilers/mikroc/pic/help/uart_library.htm#uart1_data_ready
	CALL       _UART1_Read+0
	MOVF       R0+0, 0
	MOVWF      _tempReceiveData+0
;Slave_1_Button.c,42 :: 		if(tempReceiveData == 'S')                             //received "S", enter transmitting mode
	MOVF       R0+0, 0
	XORLW      83
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt4
;Slave_1_Button.c,44 :: 		busy = 1;                                           //then UART will be busy
	MOVLW      1
	MOVWF      _busy+0
	MOVLW      0
	MOVWF      _busy+1
;Slave_1_Button.c,45 :: 		count = 0;                                          //use var count to index received Data
	CLRF       _count+0
;Slave_1_Button.c,46 :: 		receiveData[count] = tempReceiveData;
	MOVF       _count+0, 0
	ADDLW      _receiveData+0
	MOVWF      FSR
	MOVF       _tempReceiveData+0, 0
	MOVWF      INDF+0
;Slave_1_Button.c,47 :: 		count++;
	INCF       _count+0, 1
;Slave_1_Button.c,48 :: 		}
L_interrupt4:
;Slave_1_Button.c,49 :: 		if(tempReceiveData !='S' && tempReceiveData !='E')
	MOVF       _tempReceiveData+0, 0
	XORLW      83
	BTFSC      STATUS+0, 2
	GOTO       L_interrupt7
	MOVF       _tempReceiveData+0, 0
	XORLW      69
	BTFSC      STATUS+0, 2
	GOTO       L_interrupt7
L__interrupt35:
;Slave_1_Button.c,51 :: 		receiveData[count] = tempReceiveData;
	MOVF       _count+0, 0
	ADDLW      _receiveData+0
	MOVWF      FSR
	MOVF       _tempReceiveData+0, 0
	MOVWF      INDF+0
;Slave_1_Button.c,52 :: 		count++;
	INCF       _count+0, 1
;Slave_1_Button.c,53 :: 		}
L_interrupt7:
;Slave_1_Button.c,54 :: 		if(tempReceiveData == 'E')
	MOVF       _tempReceiveData+0, 0
	XORLW      69
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt8
;Slave_1_Button.c,56 :: 		receiveData[count] = tempReceiveData;
	MOVF       _count+0, 0
	ADDLW      _receiveData+0
	MOVWF      FSR
	MOVF       _tempReceiveData+0, 0
	MOVWF      INDF+0
;Slave_1_Button.c,57 :: 		count=0;
	CLRF       _count+0
;Slave_1_Button.c,58 :: 		flagReceivedAllData = 1;
	MOVLW      1
	MOVWF      _flagReceivedAllData+0
;Slave_1_Button.c,59 :: 		busy = 0;
	CLRF       _busy+0
	CLRF       _busy+1
;Slave_1_Button.c,60 :: 		}
L_interrupt8:
;Slave_1_Button.c,61 :: 		}
L_interrupt3:
;Slave_1_Button.c,62 :: 		}
L_interrupt0:
;Slave_1_Button.c,63 :: 		}
L_end_interrupt:
L__interrupt39:
	MOVF       ___savePCLATH+0, 0
	MOVWF      PCLATH+0
	SWAPF      ___saveSTATUS+0, 0
	MOVWF      STATUS+0
	SWAPF      R15+0, 1
	SWAPF      R15+0, 0
	RETFIE
; end of _interrupt

_main:

;Slave_1_Button.c,64 :: 		void main() {
;Slave_1_Button.c,65 :: 		TRISB.B0 =1;                         //bit Button 1, input. Shall become output if value = 0
	BSF        TRISB+0, 0
;Slave_1_Button.c,66 :: 		TRISB.B4 =1;                         //bit Button 2, input
	BSF        TRISB+0, 4
;Slave_1_Button.c,69 :: 		TRISB.B3 =0;                         //Bit RS485, output
	BCF        TRISB+0, 3
;Slave_1_Button.c,71 :: 		oldstate = 0;
	BCF        _oldstate+0, BitPos(_oldstate+0)
;Slave_1_Button.c,72 :: 		UART1_Init(9600);                    //init UART1 with baudrate 9600 (capable of transferring a maximum of 9600 bits per second)
	MOVLW      129
	MOVWF      SPBRG+0
	BSF        TXSTA+0, 2
	CALL       _UART1_Init+0
;Slave_1_Button.c,73 :: 		Delay_ms(100);
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_main9:
	DECFSZ     R13+0, 1
	GOTO       L_main9
	DECFSZ     R12+0, 1
	GOTO       L_main9
	DECFSZ     R11+0, 1
	GOTO       L_main9
	NOP
	NOP
;Slave_1_Button.c,75 :: 		RCIE_bit = 1;                        // enable interrupt on UART1 receive
	BSF        RCIE_bit+0, BitPos(RCIE_bit+0)
;Slave_1_Button.c,76 :: 		TXIE_bit = 0;                        // disable interrupt on UART1 transmit
	BCF        TXIE_bit+0, BitPos(TXIE_bit+0)
;Slave_1_Button.c,77 :: 		PEIE_bit = 1;                        // enable peripheral interrupts
	BSF        PEIE_bit+0, BitPos(PEIE_bit+0)
;Slave_1_Button.c,78 :: 		GIE_bit = 1;                         // enable all interrupts
	BSF        GIE_bit+0, BitPos(GIE_bit+0)
;Slave_1_Button.c,84 :: 		addressButton1[0] = '1';             //ID Button 1: 0
	MOVLW      49
	MOVWF      _addressButton1+0
;Slave_1_Button.c,85 :: 		addressButton1[1] = '0';             //ID Button 1: 1
	MOVLW      48
	MOVWF      _addressButton1+1
;Slave_1_Button.c,86 :: 		addressDevice1[0] = '0';             //ID Device 1: 0
	MOVLW      48
	MOVWF      _addressDevice1+0
;Slave_1_Button.c,87 :: 		addressDevice1[1] = '2';             //ID Device 1: 1
	MOVLW      50
	MOVWF      _addressDevice1+1
;Slave_1_Button.c,107 :: 		Delay_ms(100);
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_main10:
	DECFSZ     R13+0, 1
	GOTO       L_main10
	DECFSZ     R12+0, 1
	GOTO       L_main10
	DECFSZ     R11+0, 1
	GOTO       L_main10
	NOP
	NOP
;Slave_1_Button.c,125 :: 		while(1)
L_main11:
;Slave_1_Button.c,178 :: 		if (Button(&PORTB, 0, 1, 1)) {               // Detect logical 1 => ON device
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	CLRF       FARG_Button_pin+0
	MOVLW      1
	MOVWF      FARG_Button_time_ms+0
	MOVLW      1
	MOVWF      FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main13
;Slave_1_Button.c,179 :: 		oldstate = 1;                              // Update flag
	BSF        _oldstate+0, BitPos(_oldstate+0)
;Slave_1_Button.c,180 :: 		}
L_main13:
;Slave_1_Button.c,181 :: 		if (oldstate && Button(&PORTB, 0, 1, 0)) {   // Detect one-to-zero transition (oldstate=1) from previous if
	BTFSS      _oldstate+0, BitPos(_oldstate+0)
	GOTO       L_main16
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	CLRF       FARG_Button_pin+0
	MOVLW      1
	MOVWF      FARG_Button_time_ms+0
	CLRF       FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main16
L__main37:
;Slave_1_Button.c,182 :: 		Delay_ms(20);                             // oldstate=1 && portB turns to 0
	MOVLW      130
	MOVWF      R12+0
	MOVLW      221
	MOVWF      R13+0
L_main17:
	DECFSZ     R13+0, 1
	GOTO       L_main17
	DECFSZ     R12+0, 1
	GOTO       L_main17
	NOP
	NOP
;Slave_1_Button.c,183 :: 		if (oldstate && Button(&PORTB, 0, 1, 0))   // => OFF device using below frame
	BTFSS      _oldstate+0, BitPos(_oldstate+0)
	GOTO       L_main20
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	CLRF       FARG_Button_pin+0
	MOVLW      1
	MOVWF      FARG_Button_time_ms+0
	CLRF       FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main20
L__main36:
;Slave_1_Button.c,189 :: 		sendData[0] = 'S';
	MOVLW      83
	MOVWF      _sendData+0
;Slave_1_Button.c,190 :: 		sendData[1] = '0';
	MOVLW      48
	MOVWF      _sendData+1
;Slave_1_Button.c,191 :: 		sendData[2] = '0';
	MOVLW      48
	MOVWF      _sendData+2
;Slave_1_Button.c,192 :: 		sendData[3] = 'B';
	MOVLW      66
	MOVWF      _sendData+3
;Slave_1_Button.c,193 :: 		sendData[4] = '1';
	MOVLW      49
	MOVWF      _sendData+4
;Slave_1_Button.c,194 :: 		sendData[5] = '0';
	MOVLW      48
	MOVWF      _sendData+5
;Slave_1_Button.c,197 :: 		sendData[6] = 'D';
	MOVLW      68
	MOVWF      _sendData+6
;Slave_1_Button.c,198 :: 		sendData[7] = '0';
	MOVLW      48
	MOVWF      _sendData+7
;Slave_1_Button.c,199 :: 		sendData[8] = '2';
	MOVLW      50
	MOVWF      _sendData+8
;Slave_1_Button.c,202 :: 		sendData[9] = '0';                          //bit turn OFF
	MOVLW      48
	MOVWF      _sendData+9
;Slave_1_Button.c,203 :: 		sendData[10] = 'E';                         //end frame
	MOVLW      69
	MOVWF      _sendData+10
;Slave_1_Button.c,204 :: 		checkstt(stt1);
	MOVF       _stt1+0, 0
	MOVWF      FARG_checkstt_stt+0
	MOVF       _stt1+1, 0
	MOVWF      FARG_checkstt_stt+1
	CALL       _checkstt+0
;Slave_1_Button.c,205 :: 		stt1++;                                     //increase stt1, init stt1=1
	INCF       _stt1+0, 1
	BTFSC      STATUS+0, 2
	INCF       _stt1+1, 1
;Slave_1_Button.c,206 :: 		while(busy == 1){
L_main21:
	MOVLW      0
	XORWF      _busy+1, 0
	BTFSS      STATUS+0, 2
	GOTO       L__main41
	MOVLW      1
	XORWF      _busy+0, 0
L__main41:
	BTFSS      STATUS+0, 2
	GOTO       L_main22
;Slave_1_Button.c,208 :: 		}
	GOTO       L_main21
L_main22:
;Slave_1_Button.c,209 :: 		Delay_ms(10);
	MOVLW      65
	MOVWF      R12+0
	MOVLW      238
	MOVWF      R13+0
L_main23:
	DECFSZ     R13+0, 1
	GOTO       L_main23
	DECFSZ     R12+0, 1
	GOTO       L_main23
	NOP
;Slave_1_Button.c,210 :: 		RS485_send(sendData);
	MOVLW      _sendData+0
	MOVWF      FARG_RS485_send_dat+0
	CALL       _RS485_send+0
;Slave_1_Button.c,211 :: 		Delay_ms(100);
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_main24:
	DECFSZ     R13+0, 1
	GOTO       L_main24
	DECFSZ     R12+0, 1
	GOTO       L_main24
	DECFSZ     R11+0, 1
	GOTO       L_main24
	NOP
	NOP
;Slave_1_Button.c,213 :: 		oldstate = 0;                           // Update flag
	BCF        _oldstate+0, BitPos(_oldstate+0)
;Slave_1_Button.c,214 :: 		Delay_ms(500);
	MOVLW      13
	MOVWF      R11+0
	MOVLW      175
	MOVWF      R12+0
	MOVLW      182
	MOVWF      R13+0
L_main25:
	DECFSZ     R13+0, 1
	GOTO       L_main25
	DECFSZ     R12+0, 1
	GOTO       L_main25
	DECFSZ     R11+0, 1
	GOTO       L_main25
	NOP
;Slave_1_Button.c,215 :: 		}
L_main20:
;Slave_1_Button.c,216 :: 		}
L_main16:
;Slave_1_Button.c,217 :: 		}
	GOTO       L_main11
;Slave_1_Button.c,218 :: 		}
L_end_main:
	GOTO       $+0
; end of _main

_RS485_send:

;Slave_1_Button.c,221 :: 		void RS485_send (char dat[])
;Slave_1_Button.c,224 :: 		PORTB.RB3 =1;                        //bit RS485 on, transmitting
	BSF        PORTB+0, 3
;Slave_1_Button.c,225 :: 		Delay_ms(100);
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_RS485_send26:
	DECFSZ     R13+0, 1
	GOTO       L_RS485_send26
	DECFSZ     R12+0, 1
	GOTO       L_RS485_send26
	DECFSZ     R11+0, 1
	GOTO       L_RS485_send26
	NOP
	NOP
;Slave_1_Button.c,226 :: 		for (i=0; i<=10;i++){
	CLRF       RS485_send_i_L0+0
	CLRF       RS485_send_i_L0+1
L_RS485_send27:
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      RS485_send_i_L0+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__RS485_send43
	MOVF       RS485_send_i_L0+0, 0
	SUBLW      10
L__RS485_send43:
	BTFSS      STATUS+0, 0
	GOTO       L_RS485_send28
;Slave_1_Button.c,227 :: 		while(UART1_Tx_Idle()==0);           //while data has NOT been transmitted
L_RS485_send30:
	CALL       _UART1_Tx_Idle+0
	MOVF       R0+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_RS485_send31
	GOTO       L_RS485_send30
L_RS485_send31:
;Slave_1_Button.c,228 :: 		UART1_Write(dat[i]);                 //sendData[] passed into function and 10 char of command frame is transmitted
	MOVF       RS485_send_i_L0+0, 0
	ADDWF      FARG_RS485_send_dat+0, 0
	MOVWF      FSR
	MOVF       INDF+0, 0
	MOVWF      FARG_UART1_Write_data_+0
	CALL       _UART1_Write+0
;Slave_1_Button.c,226 :: 		for (i=0; i<=10;i++){
	INCF       RS485_send_i_L0+0, 1
	BTFSC      STATUS+0, 2
	INCF       RS485_send_i_L0+1, 1
;Slave_1_Button.c,229 :: 		}
	GOTO       L_RS485_send27
L_RS485_send28:
;Slave_1_Button.c,230 :: 		Delay_ms(100);
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_RS485_send32:
	DECFSZ     R13+0, 1
	GOTO       L_RS485_send32
	DECFSZ     R12+0, 1
	GOTO       L_RS485_send32
	DECFSZ     R11+0, 1
	GOTO       L_RS485_send32
	NOP
	NOP
;Slave_1_Button.c,231 :: 		PORTB.RB3 =0;                        //bit RS485 into idle state
	BCF        PORTB+0, 3
;Slave_1_Button.c,232 :: 		}
L_end_RS485_send:
	RETURN
; end of _RS485_send

_checkstt:

;Slave_1_Button.c,240 :: 		void checkstt (int stt)
;Slave_1_Button.c,242 :: 		if (stt % 2 == 0)       //if stt chia het cho 2 => even number
	MOVLW      2
	MOVWF      R4+0
	MOVLW      0
	MOVWF      R4+1
	MOVF       FARG_checkstt_stt+0, 0
	MOVWF      R0+0
	MOVF       FARG_checkstt_stt+1, 0
	MOVWF      R0+1
	CALL       _Div_16x16_S+0
	MOVF       R8+0, 0
	MOVWF      R0+0
	MOVF       R8+1, 0
	MOVWF      R0+1
	MOVLW      0
	XORWF      R0+1, 0
	BTFSS      STATUS+0, 2
	GOTO       L__checkstt45
	MOVLW      0
	XORWF      R0+0, 0
L__checkstt45:
	BTFSS      STATUS+0, 2
	GOTO       L_checkstt33
;Slave_1_Button.c,243 :: 		sendData[9] = '0';      //then turn OFF device (press button Twice)
	MOVLW      48
	MOVWF      _sendData+9
	GOTO       L_checkstt34
L_checkstt33:
;Slave_1_Button.c,245 :: 		sendData[9] = '1';      //else turn ON device (press button Once)
	MOVLW      49
	MOVWF      _sendData+9
L_checkstt34:
;Slave_1_Button.c,246 :: 		}
L_end_checkstt:
	RETURN
; end of _checkstt
