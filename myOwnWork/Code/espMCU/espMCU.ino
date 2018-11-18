#include <ArduinoJson.h>

#include <ESP8266WiFi.h>

WiFiClient client;


const char* ssid = "P536";      
const char* password = "05362014";  
const char* host = "192.168.100.10"; 

/*
const char* ssid     = "2.4G";
const char* password = "10365526";
const char* host = "";
*/
const int httpPort = 3000;
char updateFlag = 0;
int receiveFromSystemFlag = 0;

char dat_send_system[9];
//---------------------------
char dat_send_system_ss[9];
char flag_send_to_system = 0;
//------------------------
String dat_recive_system;

char setTimeFromInternetToSystem[11];
char tempDeviceTime[5];
char device1TimeOn[5],device1TimeOff[5];
char device2TimeOn[5],device2TimeOff[5];
char device3TimeOn[5],device3TimeOff[5];
char device4TimeOn[5],device4TimeOff[5];
char flagUpdateSetTime = 0 ;

void wifiInit(void);
void configState(void);
//System
void processDataFromSystem(void);
//Internet
void checkUpdateFlag(void);
void clearUpdateFlag(void);
void controlDevice(String device, String state);
void readJSONFromStatePage (void);
void sendTemperature(int temp);
//void sendState(void);
void sendStateFromInternetToSystem(void);
void sendSetTimeFromInternetToSystem(void);
void sendDataFromSensorToInternet(String type, int value);

void setup() {
    Serial.begin(9600);
    Serial.setTimeout(50);
    delay(100);
    wifiInit();
    configState();
    delay(200);
}

void loop() {
   // checkUpdateFlag();
    //if(updateFlag == 1){
      readJSONFromStatePage();
      if(flag_send_to_system == 1){
          flag_send_to_system = 0;
          sendStateFromInternetToSystem();
          clearUpdateFlag();
        }
      
      //sendSetTimeFromInternetToSystem();
      
     // updateFlag = 0;
    //}
    
    if(Serial.available() > 0){
       dat_recive_system = Serial.readString();
       processDataFromSystem();
    }
    
}

void wifiInit(void){
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      //Serial.print(".");
    }
    
    Serial.println("WiFi connected");  
    
}

void configState(void){

    dat_send_system[0] = 'S';
    dat_send_system[1] = '0';
    dat_send_system[2] = '0';
    dat_send_system[3] = '0';
    dat_send_system[4] = '0';
    dat_send_system[5] = '0';
    dat_send_system[6] = '0';
    dat_send_system[7] = '0';
    dat_send_system[8] = 'E';

    dat_send_system_ss[0] = 'S';
    dat_send_system_ss[1] = '0';
    dat_send_system_ss[2] = '0';
    dat_send_system_ss[3] = '0';
    dat_send_system_ss[4] = '0';
    dat_send_system_ss[5] = '0';
    dat_send_system_ss[6] = '0';
    dat_send_system_ss[7] = '0';
    dat_send_system_ss[8] = 'E';
    
    
    device1TimeOn[0]='0';
    device1TimeOn[1]='0';
    device1TimeOn[2]=':';
    device1TimeOn[3]='0';
    device1TimeOn[4]='0';
    strcpy(device1TimeOff,device1TimeOn);
    strcpy(device2TimeOn,device1TimeOn);
    strcpy(device2TimeOff,device1TimeOn);
    strcpy(device3TimeOn,device1TimeOn);
    strcpy(device3TimeOff,device1TimeOn);
    strcpy(device4TimeOn,device1TimeOn);
    strcpy(device4TimeOff,device1TimeOn);
}


void checkUpdateFlag (void)
{
    if (!client.connect(host, httpPort)) { 
    //Serial.println("Khong ket noi duoc");
    return;
    }
    client.print(String("GET /") + "checkChangedFlag" +" HTTP/1.1\r\n" +
              "Host: " + host + "\r\n" +
              "Connection: close\r\n\r\n"); 
    
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          //Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
    }

    while (client.available()) {
        String line = client.readStringUntil('\R');
        String result = line.substring(118);
        int size = result.length()+1;
        char json[size];
        result.toCharArray(json, size);
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& json_parsed = jsonBuffer.parseObject(json);
        if (strcmp(json_parsed["changedFlag"], "true") == 0) { 
            updateFlag = 1;
        }
    }
}

void clearUpdateFlag(void){
    String clearUpdateFlag = "checkChangedFlag?device=NodeMCU";
    if (!client.connect(host, httpPort)) { 
      //Serial.println("Khong ket noi duoc");
      return;
    }
    client.print(String("GET /") + clearUpdateFlag +" HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "Connection: close\r\n\r\n");             
    //delay(100);
}

void readJSONFromStatePage (void)
{
    if (!client.connect(host, httpPort)) { 
        return;
    }
    client.print(String("GET /") + "State" +" HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          //Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
    }  
          
    while (client.available()) {
        String line = client.readStringUntil('\R');
        String result = line.substring(118);
        int size = result.length()+1;
        char json[size];
        result.toCharArray(json, size);
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& json_parsed = jsonBuffer.parseObject(json);
        if (strcmp(json_parsed["TB1"], "on") == 0){  
            dat_send_system[5] = '1';
        }
        if (strcmp(json_parsed["TB1"], "off") == 0){ 
            dat_send_system[5]  = '0';
        }
        if (strcmp(json_parsed["TB2"], "on") == 0){ 
            dat_send_system[6]  = '1';
        }
        if (strcmp(json_parsed["TB2"], "off") == 0){ 
            dat_send_system[6]  = '0';   
        }
        if (strcmp(json_parsed["TB3"], "on") == 0){ 
            dat_send_system[7]  = '1';
        }
        if (strcmp(json_parsed["TB3"], "off") == 0){ 
            dat_send_system[7]  = '0';
        }
        if (strcmp(json_parsed["TB4"], "on") == 0){ 
            dat_send_system[4]  = '1';
        }
        if (strcmp(json_parsed["TB4"], "off") == 0){ 
            dat_send_system[4]  = '0';
        }


        if(strcmp(dat_send_system_ss,dat_send_system) != 0){
            strcpy(dat_send_system_ss,dat_send_system);
            flag_send_to_system = 1;
            
          }
        
        strcpy(tempDeviceTime, json_parsed["TB1timeon"]);
        if (strcmp(device1TimeOn, tempDeviceTime) !=0){
            strcpy(device1TimeOn,tempDeviceTime);
            flagUpdateSetTime = 1;
        }
        strcpy(tempDeviceTime, json_parsed["TB1timeoff"]);
        if (strcmp(device1TimeOff, tempDeviceTime) !=0){
            strcpy(device1TimeOff,tempDeviceTime);
            flagUpdateSetTime = 2;
        }
        strcpy(tempDeviceTime, json_parsed["TB2timeon"]);
        if (strcmp(device2TimeOn, tempDeviceTime) !=0){
            strcpy(device2TimeOn,tempDeviceTime);
            flagUpdateSetTime = 3;
        }
        strcpy(tempDeviceTime, json_parsed["TB2timeoff"]);
        if (strcmp(device2TimeOff, tempDeviceTime) !=0){
            strcpy(device2TimeOff,tempDeviceTime);
            flagUpdateSetTime = 4;
        }
        strcpy(tempDeviceTime, json_parsed["TB3timeon"]);
        if (strcmp(device3TimeOn, tempDeviceTime) !=0){
            strcpy(device3TimeOn,tempDeviceTime);
            flagUpdateSetTime = 5;
        }
        strcpy(tempDeviceTime, json_parsed["TB3timeoff"]);
        if (strcmp(device3TimeOff, tempDeviceTime) !=0){
            strcpy(device3TimeOff,tempDeviceTime);
            flagUpdateSetTime = 6;
        }
        strcpy(tempDeviceTime, json_parsed["TB4timeon"]);
        if (strcmp(device4TimeOn, tempDeviceTime) !=0){
            strcpy(device4TimeOn,tempDeviceTime);
            flagUpdateSetTime = 7;
        }
        strcpy(tempDeviceTime, json_parsed["TB4timeoff"]);
        if (strcmp(device4TimeOff, tempDeviceTime) !=0){
            strcpy(device4TimeOff,tempDeviceTime);
            flagUpdateSetTime = 8;
        }
        
    }
}

void sendStateFromInternetToSystem(void){
      dat_send_system[0] = 'S';
      dat_send_system[1] = '1';
      dat_send_system[2] = 'I';
      dat_send_system[3] = '1';
      Serial.println(dat_send_system);
      //delay(100);
}
void sendSetTimeFromInternetToSystem(void){
    //Device 1 set time on
    if(flagUpdateSetTime == 1){
      flagUpdateSetTime = 0;
      dat_send_system[1] = '1';
      dat_send_system[2]=device1TimeOn[0];
      dat_send_system[3]=device1TimeOn[1];
      dat_send_system[4]=device1TimeOn[3];
      dat_send_system[5]=device1TimeOn[4];
      Serial.println(dat_send_system);
      //delay(100);
    }
      //
    if(flagUpdateSetTime == 2){
      flagUpdateSetTime = 0;
      dat_send_system[1] = '2';
      dat_send_system[2]=device1TimeOn[0];
      dat_send_system[3]=device1TimeOn[1];
      dat_send_system[4]=device1TimeOn[3];
      dat_send_system[5]=device1TimeOn[4];
      Serial.println(dat_send_system);
      //delay(100);
    }
    //Device 2 set time on
    if(flagUpdateSetTime == 3){
      flagUpdateSetTime = 0;
      dat_send_system[1] = '3';
      dat_send_system[2]=device1TimeOn[0];
      dat_send_system[3]=device1TimeOn[1];
      dat_send_system[4]=device1TimeOn[3];
      dat_send_system[5]=device1TimeOn[4];
      Serial.println(dat_send_system);
      //delay(100);
    }
      //
     if(flagUpdateSetTime == 4){
      flagUpdateSetTime = 0;
      dat_send_system[1] = '4';
      dat_send_system[2]=device1TimeOn[0];
      dat_send_system[3]=device1TimeOn[1];
      dat_send_system[4]=device1TimeOn[3];
      dat_send_system[5]=device1TimeOn[4];
      Serial.println(dat_send_system);
      //delay(100);
    }
    // Device 3
    if(flagUpdateSetTime == 5){
      flagUpdateSetTime = 0;
      dat_send_system[1] = '5';
      dat_send_system[2]=device1TimeOn[0];
      dat_send_system[3]=device1TimeOn[1];
      dat_send_system[4]=device1TimeOn[3];
      dat_send_system[5]=device1TimeOn[4];
      Serial.println(dat_send_system);
      //delay(100);
    }
    if(flagUpdateSetTime == 6){
      flagUpdateSetTime = 0;
      dat_send_system[1] = '6';
      dat_send_system[2]=device1TimeOn[0];
      dat_send_system[3]=device1TimeOn[1];
      dat_send_system[4]=device1TimeOn[3];
      dat_send_system[5]=device1TimeOn[4];
      Serial.println(dat_send_system);
      //delay(100);
    }
    //device 4
    if(flagUpdateSetTime == 7){
      flagUpdateSetTime = 0;
      dat_send_system[1] = '7';
      dat_send_system[2]=device1TimeOn[0];
      dat_send_system[3]=device1TimeOn[1];
      dat_send_system[4]=device1TimeOn[3];
      dat_send_system[5]=device1TimeOn[4];
      Serial.println(dat_send_system);
      //delay(100);
    }
    if(flagUpdateSetTime == 8){
      flagUpdateSetTime = 0;
      dat_send_system[1] = '8';
      dat_send_system[2]=device1TimeOn[0];
      dat_send_system[3]=device1TimeOn[1];
      dat_send_system[4]=device1TimeOn[3];
      dat_send_system[5]=device1TimeOn[4];
      Serial.println(dat_send_system);
      //delay(100);
    }
}

void controlDevice(String device, String state){
    String url = "trangthaiTB";
    url += "?";
    url += device;
    url += "=";
    url += state;
    if (!client.connect(host, httpPort)) { 
      return;
    }
    client.print(String("GET /") + url +" HTTP/1.1\r\n" +
              "Host: " + host + "\r\n" +
              "Connection: close\r\n\r\n");             
    //delay(100);
}
/*
void sendDataFromSensorToInternet(String type, int value){
    String url;
    if(type == "Temp"){
      url += "readTempFromSystem?temperature=";
      url += value;
    }
    if(type == "Humid"){
      url += "readHumidFromSystem?humid=";
      url += value;
    }
    if(type == "Power"){
      url += "readPowerFromSystem?Power=";
      url += value;
    }
    if(type == "Gas"){
      url += "readGasFromSystem?gasDetection=";
      if(value == 1)
        url += "YES";
      else
        url += "NO";
    }
    if(type == "Human"){
      url += "readHumanFromSystem?humanDetection=";
      if(value == 1)
        url += "YES";
      else
        url += "NO";
    }
    if (!client.connect(host, httpPort)) { 
      Serial.println("Khong ket noi duoc");
      return;
    }
    client.print(String("GET /") + url +" HTTP/1.1\r\n" +
              "Host: " + host + "\r\n" +
              "Connection: close\r\n\r\n");             
    delay(200);
}
*/
void processDataFromSystem(void){
unsigned char i = 0;
    for(i = 0; i < 9;i++){
      if((dat_recive_system[i] == 'S')&&(dat_recive_system[i+1] == '0')&&(dat_recive_system[i+2] == 'I'))
      {
        break;
      }
    }
    
       if(dat_recive_system[i+3] == '1')
       {
         if(dat_recive_system[i+5] == '1'){
            controlDevice("TB1","on");
         }
         if(dat_recive_system[i+5] == '0' ){
            controlDevice("TB1","off");
         }
         //
         if(dat_recive_system[i+6] == '1'){
            controlDevice("TB2","on");
         }
         if(dat_recive_system[i+6] == '0' ){
            controlDevice("TB2","off");
         }
         //
         if(dat_recive_system[i+7] == '1'){
            controlDevice("TB3","on");
         }
         if(dat_recive_system[i+7] == '0' ){
            controlDevice("TB3","off");
         }
         //
         if(dat_recive_system[i+4] == '1'){
            controlDevice("TB4","on");
         }
         if(dat_recive_system[i+4] == '0' ){
            controlDevice("TB4","off");
         }
      }
        
      
      
  /*
    if(dat_recive_system[0] == 'S'&&dat_recive_system[1] == '0'&&dat_recive_system[2] == 'I'){
      if(dat_recive_system[3] == '1'){
         if(dat_recive_system[5] == '1'){
            controlDevice("TB1","on");
         }
         if(dat_recive_system[5] == '0' ){
            controlDevice("TB1","off");
         }
         //
         if(dat_recive_system[6] == '1'){
            controlDevice("TB2","on");
         }
         if(dat_recive_system[6] == '0' ){
            controlDevice("TB2","off");
         }
         //
         if(dat_recive_system[7] == '1'){
            controlDevice("TB3","on");
         }
         if(dat_recive_system[7] == '0' ){
            controlDevice("TB3","off");
         }
         //
         if(dat_recive_system[4] == '1'){
            controlDevice("TB4","on");
         }
         if(dat_recive_system[4] == '0' ){
            controlDevice("TB4","off");
         }
      }
    }
    */
}
