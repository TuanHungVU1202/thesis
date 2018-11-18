
_interrupt:
	MOVWF      R15+0
	SWAPF      STATUS+0, 0
	CLRF       STATUS+0
	MOVWF      ___saveSTATUS+0
	MOVF       PCLATH+0, 0
	MOVWF      ___savePCLATH+0
	CLRF       PCLATH+0

;Slave_2_Buttons.c,33 :: 		void interrupt()
;Slave_2_Buttons.c,35 :: 		if(PIR1.RCIF)
	BTFSS      PIR1+0, 5
	GOTO       L_interrupt0
;Slave_2_Buttons.c,37 :: 		while(uart1_data_ready()==0);                             //while NO data in receive register
L_interrupt1:
	CALL       _UART1_Data_Ready+0
	MOVF       R0+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt2
	GOTO       L_interrupt1
L_interrupt2:
;Slave_2_Buttons.c,38 :: 		if(uart1_data_ready()==1)                                 //if data is ready for reading
	CALL       _UART1_Data_Ready+0
	MOVF       R0+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt3
;Slave_2_Buttons.c,40 :: 		tempReceiveData = UART1_Read();                        //see https://download.mikroe.com/documents/compilers/mikroc/pic/help/uart_library.htm#uart1_data_ready
	CALL       _UART1_Read+0
	MOVF       R0+0, 0
	MOVWF      _tempReceiveData+0
;Slave_2_Buttons.c,41 :: 		if(tempReceiveData == 'S')                             //received "S", enter transmitting mode
	MOVF       R0+0, 0
	XORLW      83
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt4
;Slave_2_Buttons.c,43 :: 		busy = 1;                                           //then UART will be busy
	MOVLW      1
	MOVWF      _busy+0
	MOVLW      0
	MOVWF      _busy+1
;Slave_2_Buttons.c,44 :: 		count = 0;                                          //use var count to index received Data
	CLRF       _count+0
;Slave_2_Buttons.c,45 :: 		receiveData[count] = tempReceiveData;
	MOVF       _count+0, 0
	ADDLW      _receiveData+0
	MOVWF      FSR
	MOVF       _tempReceiveData+0, 0
	MOVWF      INDF+0
;Slave_2_Buttons.c,46 :: 		count++;
	INCF       _count+0, 1
;Slave_2_Buttons.c,47 :: 		}
L_interrupt4:
;Slave_2_Buttons.c,48 :: 		if(tempReceiveData !='S' && tempReceiveData !='E')
	MOVF       _tempReceiveData+0, 0
	XORLW      83
	BTFSC      STATUS+0, 2
	GOTO       L_interrupt7
	MOVF       _tempReceiveData+0, 0
	XORLW      69
	BTFSC      STATUS+0, 2
	GOTO       L_interrupt7
L__interrupt61:
;Slave_2_Buttons.c,50 :: 		receiveData[count] = tempReceiveData;
	MOVF       _count+0, 0
	ADDLW      _receiveData+0
	MOVWF      FSR
	MOVF       _tempReceiveData+0, 0
	MOVWF      INDF+0
;Slave_2_Buttons.c,51 :: 		count++;
	INCF       _count+0, 1
;Slave_2_Buttons.c,52 :: 		}
L_interrupt7:
;Slave_2_Buttons.c,53 :: 		if(tempReceiveData == 'E')
	MOVF       _tempReceiveData+0, 0
	XORLW      69
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt8
;Slave_2_Buttons.c,55 :: 		receiveData[count] = tempReceiveData;
	MOVF       _count+0, 0
	ADDLW      _receiveData+0
	MOVWF      FSR
	MOVF       _tempReceiveData+0, 0
	MOVWF      INDF+0
;Slave_2_Buttons.c,56 :: 		count=0;
	CLRF       _count+0
;Slave_2_Buttons.c,57 :: 		flagReceivedAllData = 1;
	MOVLW      1
	MOVWF      _flagReceivedAllData+0
;Slave_2_Buttons.c,58 :: 		busy = 0;
	CLRF       _busy+0
	CLRF       _busy+1
;Slave_2_Buttons.c,59 :: 		}
L_interrupt8:
;Slave_2_Buttons.c,60 :: 		}
L_interrupt3:
;Slave_2_Buttons.c,61 :: 		}
L_interrupt0:
;Slave_2_Buttons.c,62 :: 		}
L_end_interrupt:
L__interrupt69:
	MOVF       ___savePCLATH+0, 0
	MOVWF      PCLATH+0
	SWAPF      ___saveSTATUS+0, 0
	MOVWF      STATUS+0
	SWAPF      R15+0, 1
	SWAPF      R15+0, 0
	RETFIE
; end of _interrupt

_main:

;Slave_2_Buttons.c,63 :: 		void main() {
;Slave_2_Buttons.c,64 :: 		TRISB.B0 =1;                         //bit Button 1, input. Shall become output if value = 0
	BSF        TRISB+0, 0
;Slave_2_Buttons.c,65 :: 		TRISB.B4 =1;                         //bit Button 2, input
	BSF        TRISB+0, 4
;Slave_2_Buttons.c,66 :: 		TRISB.B5 =1;                         //bit Button 3, intput
	BSF        TRISB+0, 5
;Slave_2_Buttons.c,68 :: 		TRISB.B3 =0;                         //Bit RS485, output
	BCF        TRISB+0, 3
;Slave_2_Buttons.c,70 :: 		oldstate = 0;
	BCF        _oldstate+0, BitPos(_oldstate+0)
;Slave_2_Buttons.c,71 :: 		UART1_Init(9600);                    //init UART1 with baudrate 9600 (capable of transferring a maximum of 9600 bits per second)
	MOVLW      129
	MOVWF      SPBRG+0
	BSF        TXSTA+0, 2
	CALL       _UART1_Init+0
;Slave_2_Buttons.c,72 :: 		Delay_ms(100);
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
;Slave_2_Buttons.c,74 :: 		RCIE_bit = 1;                        // enable interrupt on UART1 receive
	BSF        RCIE_bit+0, BitPos(RCIE_bit+0)
;Slave_2_Buttons.c,75 :: 		TXIE_bit = 0;                        // disable interrupt on UART1 transmit
	BCF        TXIE_bit+0, BitPos(TXIE_bit+0)
;Slave_2_Buttons.c,76 :: 		PEIE_bit = 1;                        // enable peripheral interrupts
	BSF        PEIE_bit+0, BitPos(PEIE_bit+0)
;Slave_2_Buttons.c,77 :: 		GIE_bit = 1;                         // enable all interrupts
	BSF        GIE_bit+0, BitPos(GIE_bit+0)
;Slave_2_Buttons.c,83 :: 		addressButton1[0] = '0';             //ID Button 1: 0
	MOVLW      48
	MOVWF      _addressButton1+0
;Slave_2_Buttons.c,84 :: 		addressButton1[1] = '4';             //ID Button 1: 1
	MOVLW      52
	MOVWF      _addressButton1+1
;Slave_2_Buttons.c,85 :: 		addressDevice1[0] = '0';             //ID Device 1: 0
	MOVLW      48
	MOVWF      _addressDevice1+0
;Slave_2_Buttons.c,86 :: 		addressDevice1[1] = '4';             //ID Device 1: 1
	MOVLW      52
	MOVWF      _addressDevice1+1
;Slave_2_Buttons.c,88 :: 		addressButton2[0] = '0';
	MOVLW      48
	MOVWF      _addressButton2+0
;Slave_2_Buttons.c,89 :: 		addressButton2[1] = '5';
	MOVLW      53
	MOVWF      _addressButton2+1
;Slave_2_Buttons.c,90 :: 		addressDevice2[0] = '0';
	MOVLW      48
	MOVWF      _addressDevice2+0
;Slave_2_Buttons.c,91 :: 		addressDevice2[1] = '5';
	MOVLW      53
	MOVWF      _addressDevice2+1
;Slave_2_Buttons.c,93 :: 		addressButton3[0] = '0';
	MOVLW      48
	MOVWF      _addressButton3+0
;Slave_2_Buttons.c,94 :: 		addressButton3[1] = '6';
	MOVLW      54
	MOVWF      _addressButton3+1
;Slave_2_Buttons.c,95 :: 		addressDevice3[0] = '0';
	MOVLW      48
	MOVWF      _addressDevice3+0
;Slave_2_Buttons.c,96 :: 		addressDevice3[1] = '6';
	MOVLW      54
	MOVWF      _addressDevice3+1
;Slave_2_Buttons.c,115 :: 		Delay_ms(100);
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
;Slave_2_Buttons.c,133 :: 		while(1)
L_main11:
;Slave_2_Buttons.c,186 :: 		if (Button(&PORTB, 0, 1, 1)) {               // Detect logical 1 => ON device
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
;Slave_2_Buttons.c,187 :: 		oldstate = 1;                              // Update flag
	BSF        _oldstate+0, BitPos(_oldstate+0)
;Slave_2_Buttons.c,188 :: 		}
L_main13:
;Slave_2_Buttons.c,189 :: 		if (oldstate && Button(&PORTB, 0, 1, 0)) {   // Detect one-to-zero transition (oldstate=1) from previous if
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
L__main67:
;Slave_2_Buttons.c,190 :: 		Delay_ms(100);                             // oldstate=1 && portB turns to 0
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_main17:
	DECFSZ     R13+0, 1
	GOTO       L_main17
	DECFSZ     R12+0, 1
	GOTO       L_main17
	DECFSZ     R11+0, 1
	GOTO       L_main17
	NOP
	NOP
;Slave_2_Buttons.c,191 :: 		if (oldstate && Button(&PORTB, 0, 1, 0))   // => OFF device using below frame
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
L__main66:
;Slave_2_Buttons.c,197 :: 		sendData[0] = 'S';
	MOVLW      83
	MOVWF      _sendData+0
;Slave_2_Buttons.c,198 :: 		sendData[1] = '0';
	MOVLW      48
	MOVWF      _sendData+1
;Slave_2_Buttons.c,199 :: 		sendData[2] = '0';
	MOVLW      48
	MOVWF      _sendData+2
;Slave_2_Buttons.c,200 :: 		sendData[3] = 'B';
	MOVLW      66
	MOVWF      _sendData+3
;Slave_2_Buttons.c,201 :: 		sendData[4] = '0';
	MOVLW      48
	MOVWF      _sendData+4
;Slave_2_Buttons.c,202 :: 		sendData[5] = '4';
	MOVLW      52
	MOVWF      _sendData+5
;Slave_2_Buttons.c,205 :: 		sendData[6] = 'D';
	MOVLW      68
	MOVWF      _sendData+6
;Slave_2_Buttons.c,206 :: 		sendData[7] = '0';
	MOVLW      48
	MOVWF      _sendData+7
;Slave_2_Buttons.c,207 :: 		sendData[8] = '4';
	MOVLW      52
	MOVWF      _sendData+8
;Slave_2_Buttons.c,210 :: 		sendData[9] = '0';                          //bit turn OFF
	MOVLW      48
	MOVWF      _sendData+9
;Slave_2_Buttons.c,211 :: 		sendData[10] = 'E';                         //end frame
	MOVLW      69
	MOVWF      _sendData+10
;Slave_2_Buttons.c,212 :: 		checkstt(stt1);
	MOVF       _stt1+0, 0
	MOVWF      FARG_checkstt_stt+0
	MOVF       _stt1+1, 0
	MOVWF      FARG_checkstt_stt+1
	CALL       _checkstt+0
;Slave_2_Buttons.c,213 :: 		stt1++;                                     //increase stt1, init stt1=1
	INCF       _stt1+0, 1
	BTFSC      STATUS+0, 2
	INCF       _stt1+1, 1
;Slave_2_Buttons.c,214 :: 		while(busy == 1){
L_main21:
	MOVLW      0
	XORWF      _busy+1, 0
	BTFSS      STATUS+0, 2
	GOTO       L__main71
	MOVLW      1
	XORWF      _busy+0, 0
L__main71:
	BTFSS      STATUS+0, 2
	GOTO       L_main22
;Slave_2_Buttons.c,216 :: 		}
	GOTO       L_main21
L_main22:
;Slave_2_Buttons.c,217 :: 		Delay_ms(10);
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
;Slave_2_Buttons.c,218 :: 		RS485_send(sendData);
	MOVLW      _sendData+0
	MOVWF      FARG_RS485_send_dat+0
	CALL       _RS485_send+0
;Slave_2_Buttons.c,219 :: 		Delay_ms(100);
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
;Slave_2_Buttons.c,221 :: 		oldstate = 0;                           // Update flag
	BCF        _oldstate+0, BitPos(_oldstate+0)
;Slave_2_Buttons.c,222 :: 		Delay_ms(500);
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
;Slave_2_Buttons.c,223 :: 		}
L_main20:
;Slave_2_Buttons.c,224 :: 		}
L_main16:
;Slave_2_Buttons.c,227 :: 		if (Button(&PORTB, 4, 1, 1)) {               // Detect logical one => ON device
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	MOVLW      4
	MOVWF      FARG_Button_pin+0
	MOVLW      1
	MOVWF      FARG_Button_time_ms+0
	MOVLW      1
	MOVWF      FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main26
;Slave_2_Buttons.c,228 :: 		oldstate = 1;                              // Update flag
	BSF        _oldstate+0, BitPos(_oldstate+0)
;Slave_2_Buttons.c,229 :: 		}
L_main26:
;Slave_2_Buttons.c,230 :: 		if (oldstate && Button(&PORTB, 4, 1, 0)) {   // Detect one-to-zero transition => OFF device using below frame
	BTFSS      _oldstate+0, BitPos(_oldstate+0)
	GOTO       L_main29
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	MOVLW      4
	MOVWF      FARG_Button_pin+0
	MOVLW      1
	MOVWF      FARG_Button_time_ms+0
	CLRF       FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main29
L__main65:
;Slave_2_Buttons.c,231 :: 		Delay_ms(100);
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_main30:
	DECFSZ     R13+0, 1
	GOTO       L_main30
	DECFSZ     R12+0, 1
	GOTO       L_main30
	DECFSZ     R11+0, 1
	GOTO       L_main30
	NOP
	NOP
;Slave_2_Buttons.c,232 :: 		if (oldstate && Button(&PORTB, 4, 1, 0))
	BTFSS      _oldstate+0, BitPos(_oldstate+0)
	GOTO       L_main33
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	MOVLW      4
	MOVWF      FARG_Button_pin+0
	MOVLW      1
	MOVWF      FARG_Button_time_ms+0
	CLRF       FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main33
L__main64:
;Slave_2_Buttons.c,234 :: 		sendData[0] = 'S';
	MOVLW      83
	MOVWF      _sendData+0
;Slave_2_Buttons.c,235 :: 		sendData[1] = '0';
	MOVLW      48
	MOVWF      _sendData+1
;Slave_2_Buttons.c,236 :: 		sendData[2] = '0';
	MOVLW      48
	MOVWF      _sendData+2
;Slave_2_Buttons.c,237 :: 		sendData[3] = 'B';
	MOVLW      66
	MOVWF      _sendData+3
;Slave_2_Buttons.c,242 :: 		sendData[4] = '0';
	MOVLW      48
	MOVWF      _sendData+4
;Slave_2_Buttons.c,243 :: 		sendData[5] = '5';
	MOVLW      53
	MOVWF      _sendData+5
;Slave_2_Buttons.c,244 :: 		sendData[6] = 'D';
	MOVLW      68
	MOVWF      _sendData+6
;Slave_2_Buttons.c,249 :: 		sendData[7] = '0';
	MOVLW      48
	MOVWF      _sendData+7
;Slave_2_Buttons.c,250 :: 		sendData[8] = '5';
	MOVLW      53
	MOVWF      _sendData+8
;Slave_2_Buttons.c,251 :: 		sendData[9] = '0';
	MOVLW      48
	MOVWF      _sendData+9
;Slave_2_Buttons.c,252 :: 		sendData[10] = 'E';
	MOVLW      69
	MOVWF      _sendData+10
;Slave_2_Buttons.c,253 :: 		checkstt(stt2);
	MOVF       _stt2+0, 0
	MOVWF      FARG_checkstt_stt+0
	MOVF       _stt2+1, 0
	MOVWF      FARG_checkstt_stt+1
	CALL       _checkstt+0
;Slave_2_Buttons.c,254 :: 		stt2++;
	INCF       _stt2+0, 1
	BTFSC      STATUS+0, 2
	INCF       _stt2+1, 1
;Slave_2_Buttons.c,255 :: 		while(busy == 1){
L_main34:
	MOVLW      0
	XORWF      _busy+1, 0
	BTFSS      STATUS+0, 2
	GOTO       L__main72
	MOVLW      1
	XORWF      _busy+0, 0
L__main72:
	BTFSS      STATUS+0, 2
	GOTO       L_main35
;Slave_2_Buttons.c,257 :: 		}
	GOTO       L_main34
L_main35:
;Slave_2_Buttons.c,258 :: 		Delay_ms(10);
	MOVLW      65
	MOVWF      R12+0
	MOVLW      238
	MOVWF      R13+0
L_main36:
	DECFSZ     R13+0, 1
	GOTO       L_main36
	DECFSZ     R12+0, 1
	GOTO       L_main36
	NOP
;Slave_2_Buttons.c,259 :: 		RS485_send(sendData);
	MOVLW      _sendData+0
	MOVWF      FARG_RS485_send_dat+0
	CALL       _RS485_send+0
;Slave_2_Buttons.c,260 :: 		Delay_ms(100);
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_main37:
	DECFSZ     R13+0, 1
	GOTO       L_main37
	DECFSZ     R12+0, 1
	GOTO       L_main37
	DECFSZ     R11+0, 1
	GOTO       L_main37
	NOP
	NOP
;Slave_2_Buttons.c,262 :: 		oldstate = 0;                           // Update flag
	BCF        _oldstate+0, BitPos(_oldstate+0)
;Slave_2_Buttons.c,263 :: 		Delay_ms(500);
	MOVLW      13
	MOVWF      R11+0
	MOVLW      175
	MOVWF      R12+0
	MOVLW      182
	MOVWF      R13+0
L_main38:
	DECFSZ     R13+0, 1
	GOTO       L_main38
	DECFSZ     R12+0, 1
	GOTO       L_main38
	DECFSZ     R11+0, 1
	GOTO       L_main38
	NOP
;Slave_2_Buttons.c,264 :: 		}
L_main33:
;Slave_2_Buttons.c,265 :: 		}
L_main29:
;Slave_2_Buttons.c,268 :: 		if (Button(&PORTB, 5, 1, 1)) {               // Detect logical one => ON device
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	MOVLW      5
	MOVWF      FARG_Button_pin+0
	MOVLW      1
	MOVWF      FARG_Button_time_ms+0
	MOVLW      1
	MOVWF      FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main39
;Slave_2_Buttons.c,269 :: 		oldstate = 1;                              // Update flag
	BSF        _oldstate+0, BitPos(_oldstate+0)
;Slave_2_Buttons.c,270 :: 		}
L_main39:
;Slave_2_Buttons.c,272 :: 		if (oldstate && Button(&PORTB, 5, 1, 0)) {   // Detect one-to-zero transition => OFF device using below frame
	BTFSS      _oldstate+0, BitPos(_oldstate+0)
	GOTO       L_main42
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	MOVLW      5
	MOVWF      FARG_Button_pin+0
	MOVLW      1
	MOVWF      FARG_Button_time_ms+0
	CLRF       FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main42
L__main63:
;Slave_2_Buttons.c,273 :: 		Delay_ms(100);
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_main43:
	DECFSZ     R13+0, 1
	GOTO       L_main43
	DECFSZ     R12+0, 1
	GOTO       L_main43
	DECFSZ     R11+0, 1
	GOTO       L_main43
	NOP
	NOP
;Slave_2_Buttons.c,274 :: 		if (oldstate && Button(&PORTB, 5, 1, 0))
	BTFSS      _oldstate+0, BitPos(_oldstate+0)
	GOTO       L_main46
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	MOVLW      5
	MOVWF      FARG_Button_pin+0
	MOVLW      1
	MOVWF      FARG_Button_time_ms+0
	CLRF       FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main46
L__main62:
;Slave_2_Buttons.c,276 :: 		sendData[0] = 'S';
	MOVLW      83
	MOVWF      _sendData+0
;Slave_2_Buttons.c,277 :: 		sendData[1] = '0';
	MOVLW      48
	MOVWF      _sendData+1
;Slave_2_Buttons.c,278 :: 		sendData[2] = '0';
	MOVLW      48
	MOVWF      _sendData+2
;Slave_2_Buttons.c,279 :: 		sendData[3] = 'B';
	MOVLW      66
	MOVWF      _sendData+3
;Slave_2_Buttons.c,284 :: 		sendData[4] = '0';
	MOVLW      48
	MOVWF      _sendData+4
;Slave_2_Buttons.c,285 :: 		sendData[5] = '6';
	MOVLW      54
	MOVWF      _sendData+5
;Slave_2_Buttons.c,286 :: 		sendData[6] = 'D';
	MOVLW      68
	MOVWF      _sendData+6
;Slave_2_Buttons.c,291 :: 		sendData[7] = '0';
	MOVLW      48
	MOVWF      _sendData+7
;Slave_2_Buttons.c,292 :: 		sendData[8] = '6';
	MOVLW      54
	MOVWF      _sendData+8
;Slave_2_Buttons.c,293 :: 		sendData[9] = '0';
	MOVLW      48
	MOVWF      _sendData+9
;Slave_2_Buttons.c,294 :: 		sendData[10] = 'E';
	MOVLW      69
	MOVWF      _sendData+10
;Slave_2_Buttons.c,295 :: 		checkstt(stt3);
	MOVF       _stt3+0, 0
	MOVWF      FARG_checkstt_stt+0
	MOVF       _stt3+1, 0
	MOVWF      FARG_checkstt_stt+1
	CALL       _checkstt+0
;Slave_2_Buttons.c,296 :: 		stt3++;
	INCF       _stt3+0, 1
	BTFSC      STATUS+0, 2
	INCF       _stt3+1, 1
;Slave_2_Buttons.c,297 :: 		while(busy == 1){
L_main47:
	MOVLW      0
	XORWF      _busy+1, 0
	BTFSS      STATUS+0, 2
	GOTO       L__main73
	MOVLW      1
	XORWF      _busy+0, 0
L__main73:
	BTFSS      STATUS+0, 2
	GOTO       L_main48
;Slave_2_Buttons.c,299 :: 		}
	GOTO       L_main47
L_main48:
;Slave_2_Buttons.c,300 :: 		Delay_ms(10);
	MOVLW      65
	MOVWF      R12+0
	MOVLW      238
	MOVWF      R13+0
L_main49:
	DECFSZ     R13+0, 1
	GOTO       L_main49
	DECFSZ     R12+0, 1
	GOTO       L_main49
	NOP
;Slave_2_Buttons.c,301 :: 		RS485_send(sendData);
	MOVLW      _sendData+0
	MOVWF      FARG_RS485_send_dat+0
	CALL       _RS485_send+0
;Slave_2_Buttons.c,302 :: 		Delay_ms(100);
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_main50:
	DECFSZ     R13+0, 1
	GOTO       L_main50
	DECFSZ     R12+0, 1
	GOTO       L_main50
	DECFSZ     R11+0, 1
	GOTO       L_main50
	NOP
	NOP
;Slave_2_Buttons.c,304 :: 		oldstate = 0;
	BCF        _oldstate+0, BitPos(_oldstate+0)
;Slave_2_Buttons.c,305 :: 		Delay_ms(500);
	MOVLW      13
	MOVWF      R11+0
	MOVLW      175
	MOVWF      R12+0
	MOVLW      182
	MOVWF      R13+0
L_main51:
	DECFSZ     R13+0, 1
	GOTO       L_main51
	DECFSZ     R12+0, 1
	GOTO       L_main51
	DECFSZ     R11+0, 1
	GOTO       L_main51
	NOP
;Slave_2_Buttons.c,306 :: 		}
L_main46:
;Slave_2_Buttons.c,307 :: 		}
L_main42:
;Slave_2_Buttons.c,309 :: 		}
	GOTO       L_main11
;Slave_2_Buttons.c,310 :: 		}
L_end_main:
	GOTO       $+0
; end of _main

_RS485_send:

;Slave_2_Buttons.c,313 :: 		void RS485_send (char dat[])
;Slave_2_Buttons.c,316 :: 		PORTB.RB3 =1;                        //bit RS485 on, transmitting
	BSF        PORTB+0, 3
;Slave_2_Buttons.c,317 :: 		Delay_ms(100);
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_RS485_send52:
	DECFSZ     R13+0, 1
	GOTO       L_RS485_send52
	DECFSZ     R12+0, 1
	GOTO       L_RS485_send52
	DECFSZ     R11+0, 1
	GOTO       L_RS485_send52
	NOP
	NOP
;Slave_2_Buttons.c,318 :: 		for (i=0; i<=10;i++){
	CLRF       RS485_send_i_L0+0
	CLRF       RS485_send_i_L0+1
L_RS485_send53:
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      RS485_send_i_L0+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__RS485_send75
	MOVF       RS485_send_i_L0+0, 0
	SUBLW      10
L__RS485_send75:
	BTFSS      STATUS+0, 0
	GOTO       L_RS485_send54
;Slave_2_Buttons.c,319 :: 		while(UART1_Tx_Idle()==0);           //while data has NOT been transmitted
L_RS485_send56:
	CALL       _UART1_Tx_Idle+0
	MOVF       R0+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_RS485_send57
	GOTO       L_RS485_send56
L_RS485_send57:
;Slave_2_Buttons.c,320 :: 		UART1_Write(dat[i]);                 //sendData[] passed into function and 10 char of command frame is transmitted
	MOVF       RS485_send_i_L0+0, 0
	ADDWF      FARG_RS485_send_dat+0, 0
	MOVWF      FSR
	MOVF       INDF+0, 0
	MOVWF      FARG_UART1_Write_data_+0
	CALL       _UART1_Write+0
;Slave_2_Buttons.c,318 :: 		for (i=0; i<=10;i++){
	INCF       RS485_send_i_L0+0, 1
	BTFSC      STATUS+0, 2
	INCF       RS485_send_i_L0+1, 1
;Slave_2_Buttons.c,321 :: 		}
	GOTO       L_RS485_send53
L_RS485_send54:
;Slave_2_Buttons.c,322 :: 		Delay_ms(100);
	MOVLW      3
	MOVWF      R11+0
	MOVLW      138
	MOVWF      R12+0
	MOVLW      85
	MOVWF      R13+0
L_RS485_send58:
	DECFSZ     R13+0, 1
	GOTO       L_RS485_send58
	DECFSZ     R12+0, 1
	GOTO       L_RS485_send58
	DECFSZ     R11+0, 1
	GOTO       L_RS485_send58
	NOP
	NOP
;Slave_2_Buttons.c,323 :: 		PORTB.RB3 =0;                        //bit RS485 into idle state
	BCF        PORTB+0, 3
;Slave_2_Buttons.c,324 :: 		}
L_end_RS485_send:
	RETURN
; end of _RS485_send

_checkstt:

;Slave_2_Buttons.c,332 :: 		void checkstt (int stt)
;Slave_2_Buttons.c,334 :: 		if (stt % 2 == 0)       //if stt chia het cho 2 => even number
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
	GOTO       L__checkstt77
	MOVLW      0
	XORWF      R0+0, 0
L__checkstt77:
	BTFSS      STATUS+0, 2
	GOTO       L_checkstt59
;Slave_2_Buttons.c,335 :: 		sendData[9] = '0';      //then turn OFF device (press button Twice)
	MOVLW      48
	MOVWF      _sendData+9
	GOTO       L_checkstt60
L_checkstt59:
;Slave_2_Buttons.c,337 :: 		sendData[9] = '1';      //else turn ON device (press button Once)
	MOVLW      49
	MOVWF      _sendData+9
L_checkstt60:
;Slave_2_Buttons.c,338 :: 		}
L_end_checkstt:
	RETURN
; end of _checkstt
