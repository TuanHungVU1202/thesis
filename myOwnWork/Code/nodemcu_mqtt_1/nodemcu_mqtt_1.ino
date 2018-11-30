#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/*
const char* ssid     = "P811";
const char* password = "tumotdenchin";
const char* host = "192.168.100.13";
*/
/*
const char* ssid     = "PTNKTD2.4G";
const char* password = "10365526";
const char* host = "192.168.0.181";

const char* ssid     = "Erasmus";
const char* password = "lifein5months";
const char* host = "192.168.0.105";
*/

//setup mqtt client params
const char* ssid     = "Erasmus";
const char* password = "lifein5months";
const char* mqttServer = "192.168.0.103";
const int mqttPort = 3000;
//const char* mqttUser = "YourMqttUser";
//const char* mqttPassword = "YourMqttUserPassword";
String topic_str;
String message_str;


//init vars
int updateFlag = 0;
int receiveFromSystemFlag = 0;
int count = 0;
char stateFromInternetToSystem[18],oldstateFromInternetToSystem[18];
String stateFromSystemToInternet;
char oldstateFromSystemToInternet[18];
char devicesState[18];
char setTimeFromInternetToSystem[11];
char tempDeviceTime[5],flagUpdateSetTime[11],device1TimeOn[5],device1TimeOff[5],device2TimeOn[5],device2TimeOff[5],device3TimeOn[5],device3TimeOff[5],device4TimeOn[5],device4TimeOff[5],device5TimeOn[5],device5TimeOff[5],device6TimeOn[5],device6TimeOff[5],device7TimeOn[5],device7TimeOff[5];
 
WiFiClient espClient;
PubSubClient client(espClient);


//Start program
void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
    } else {
      Serial.println("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.publish("fromEsp/control/device/1", "Hello from ESP8266");
  
  client.subscribe("toEsp/control/device/1");
  client.subscribe("toEsp/control/device/2");
  client.subscribe("toEsp/control/device/3");
  client.subscribe("toEsp/control/device/4");
  client.subscribe("toEsp/control/device/5");
  client.subscribe("toEsp/control/device/6");
  client.subscribe("toEsp/control/device/7");

  //send Timer to System
  client.subscribe("toEsp/timer/device/1/on");
  client.subscribe("toEsp/timer/device/1/off");
  client.subscribe("toEsp/timer/device/2/on");
  client.subscribe("toEsp/timer/device/2/off");
  client.subscribe("toEsp/timer/device/3/on");
  client.subscribe("toEsp/timer/device/3/off");
  client.subscribe("toEsp/timer/device/4/on");
  client.subscribe("toEsp/timer/device/4/off");
  client.subscribe("toEsp/timer/device/5/on");
  client.subscribe("toEsp/timer/device/5/off");
  client.subscribe("toEsp/timer/device/6/on");
  client.subscribe("toEsp/timer/device/6/off");
  client.subscribe("toEsp/timer/device/7/on");
  client.subscribe("toEsp/timer/device/7/off");
  //call function to config initial States
  configState();
  delay(500);
}


//main callback
void callback(char* topic, byte* payload, unsigned int length) {
  topic_str = String(topic);
  int i = 0;
  char message_buff[3];

  /* Print out to serial to debug only
  Serial.print("Message arrived in topic: ");
  Serial.println(topic_str);
  Serial.println("Message:");
  */
  
  for (i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    message_buff[i]=payload[i];
  }

  //convert message buffer to string
  message_buff[i] = *"\0";
  message_str = String(message_buff);
  //Serial.println(message_str);
  
  //function to send command to system through esp8266
  sendDataToSystem();
  sendTimeToSystem();

  /*just for making serial monitor looks better
  Serial.println();
  Serial.println("-----------------------");
  */
}


//init states
void configState(void){
    stateFromInternetToSystem[0]  = 'S';
    for(int i=1;i<17;i++){
      stateFromInternetToSystem[i]='0';
    }
    stateFromInternetToSystem[17] = 'E';
    
    strcpy(devicesState,stateFromInternetToSystem);
    
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
    strcpy(device5TimeOn,device1TimeOn);
    strcpy(device5TimeOff,device1TimeOn);
    strcpy(device6TimeOn,device1TimeOn);
    strcpy(device6TimeOff,device1TimeOn);
    strcpy(device7TimeOn,device1TimeOn);
    strcpy(device7TimeOff,device1TimeOn);
    
    setTimeFromInternetToSystem[0] = 'S';
    setTimeFromInternetToSystem[1] = '1';
    setTimeFromInternetToSystem[2] = 'D';
    setTimeFromInternetToSystem[3] = '0';
    setTimeFromInternetToSystem[4] = '1';
    setTimeFromInternetToSystem[5] = '0';
    setTimeFromInternetToSystem[6] = '0';
    setTimeFromInternetToSystem[7] = '0';
    setTimeFromInternetToSystem[8] = '0';
    setTimeFromInternetToSystem[9] = '0';
    setTimeFromInternetToSystem[10]= 'E';
}


//SENDING DATA from INTERNET to SYSTEM (INCLUDE COMMAND DEVICES AND SET TIME ON/OFF)
//--------------------------------------------------------------------------------------------------------------------
//send command to system
void sendDataToSystem(void){
  if(topic_str == "toEsp/control/device/1"){
    if(message_str == "on"){
      stateFromInternetToSystem[2]  = '1';
      //Serial.println("Turning On D1");
      Serial.println(stateFromInternetToSystem);
    } else {
      stateFromInternetToSystem[2]  = '0';
      //Serial.println("Turning Off D1");
      Serial.println(stateFromInternetToSystem);
     }
  }
  if(topic_str == "toEsp/control/device/2"){
    if(message_str == "on"){
      stateFromInternetToSystem[3]  = '1';
      //Serial.println("Turning On D2");
      Serial.println(stateFromInternetToSystem);
    } else {
      stateFromInternetToSystem[3]  = '0'; 
      //Serial.println("Turning Off D2");
      Serial.println(stateFromInternetToSystem);
     }
  }
  if(topic_str == "toEsp/control/device/3"){
    if(message_str == "on"){
      stateFromInternetToSystem[4]  = '1';
      //Serial.println("Turning On D3");
      Serial.println(stateFromInternetToSystem);
    } else {
      stateFromInternetToSystem[4]  = '0';
      //Serial.println("Turning Off D3");
      Serial.println(stateFromInternetToSystem);
     }
  }
  if(topic_str == "toEsp/control/device/4"){
    if(message_str == "on"){
      stateFromInternetToSystem[5]  = '1';
      //Serial.println("Turning On D4");
      Serial.println(stateFromInternetToSystem);
    } else {
      stateFromInternetToSystem[5]  = '0';
      //Serial.println("Turning Off D4");
      Serial.println(stateFromInternetToSystem);
     }
  }
  if(topic_str == "toEsp/control/device/5"){
    if(message_str == "on"){
      stateFromInternetToSystem[6]  = '1';
      //Serial.println("Turning On D5");
      Serial.println(stateFromInternetToSystem);
    } else {
      stateFromInternetToSystem[6]  = '0';
      //Serial.println("Turning Off D5");
      Serial.println(stateFromInternetToSystem);
     }
  }
    if(topic_str == "toEsp/control/device/6"){
    if(message_str == "on"){
      stateFromInternetToSystem[7]  = '1';
      //Serial.println("Turning On D6");
      Serial.println(stateFromInternetToSystem);
    } else {
      stateFromInternetToSystem[7]  = '0';
      //Serial.println("Turning Off D6");
      Serial.println(stateFromInternetToSystem);
     }
  }
    if(topic_str == "toEsp/control/device/7"){
    if(message_str == "on"){
      stateFromInternetToSystem[8]  = '1';
      //Serial.println("Turning On D7");
      Serial.println(stateFromInternetToSystem);
    } else {
      stateFromInternetToSystem[8]  = '0';
      //Serial.println("Turning Off D7");
      Serial.println(stateFromInternetToSystem);
     }
  }
}


//send timer to system
void sendTimeToSystem(void){
  if(topic_str == "toEsp/timer/device/1/on"){
      setTimeFromInternetToSystem[4]='1';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='1';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }

  if(topic_str == "toEsp/timer/device/1/off"){
      setTimeFromInternetToSystem[4]='1';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='0';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }


  if(topic_str == "toEsp/timer/device/2/on"){
      setTimeFromInternetToSystem[4]='2';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='1';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }

  if(topic_str == "toEsp/timer/device/2/off"){
      setTimeFromInternetToSystem[4]='2';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='0';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }


  if(topic_str == "toEsp/timer/device/3/on"){
      setTimeFromInternetToSystem[4]='3';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='1';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }

  if(topic_str == "toEsp/timer/device/3/off"){
      setTimeFromInternetToSystem[4]='3';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='0';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }


  if(topic_str == "toEsp/timer/device/4/on"){
      setTimeFromInternetToSystem[4]='4';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='1';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }

  if(topic_str == "toEsp/timer/device/4/off"){
      setTimeFromInternetToSystem[4]='4';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='0';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }

  if(topic_str == "toEsp/timer/device/5/on"){
      setTimeFromInternetToSystem[4]='5';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='1';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }

  if(topic_str == "toEsp/timer/device/5/off"){
      setTimeFromInternetToSystem[4]='5';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='0';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }

    if(topic_str == "toEsp/timer/device/6/on"){
      setTimeFromInternetToSystem[4]='6';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='1';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }

  if(topic_str == "toEsp/timer/device/6/off"){
      setTimeFromInternetToSystem[4]='6';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='0';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }
    if(topic_str == "toEsp/timer/device/7/on"){
      setTimeFromInternetToSystem[4]='7';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='1';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }

  if(topic_str == "toEsp/timer/device/7/off"){
      setTimeFromInternetToSystem[4]='7';
      setTimeFromInternetToSystem[5]=message_str[0];
      setTimeFromInternetToSystem[6]=message_str[1];
      setTimeFromInternetToSystem[7]=message_str[3];
      setTimeFromInternetToSystem[8]=message_str[4];
      setTimeFromInternetToSystem[9]='0';
      Serial.println(setTimeFromInternetToSystem);
      delay(50);
    }
    
}
//--------------------------------------------------------------------------------------------------------------------


//SENDING DATA from SYSTEM to INTERNET (synchronization)
//--------------------------------------------------------------------------------------------------------------------
//use this to test through Serial S010000000000000E
//process data from system (include data from sensors
void processDataFromSystem(void){
    int Temp, Humid, Gas, Human, Power;
    if(stateFromSystemToInternet[1] == '0'){
       if(stateFromSystemToInternet[2] == '1' && stateFromSystemToInternet[2] != stateFromInternetToSystem[2]){
          stateFromInternetToSystem[2] = stateFromSystemToInternet[2];
          client.publish("fromEsp/control/device/1", "on");
          //sendDataToInternet("device1","on");
       }
       if(stateFromSystemToInternet[2] == '0' && stateFromSystemToInternet[2] != stateFromInternetToSystem[2]){
          stateFromInternetToSystem[2] = stateFromSystemToInternet[2];
          client.publish("fromEsp/control/device/1", "off");
          //sendDataToInternet("device1","off");
       }
       if(stateFromSystemToInternet[3] == '1' && stateFromSystemToInternet[3] != stateFromInternetToSystem[3]){
          stateFromInternetToSystem[3] = stateFromSystemToInternet[3];
          client.publish("fromEsp/control/device/2", "on");
          //sendDataToInternet("device2","on");
       }
       if(stateFromSystemToInternet[3] == '0' && stateFromSystemToInternet[3] != stateFromInternetToSystem[3]){
          stateFromInternetToSystem[3] = stateFromSystemToInternet[3];
          client.publish("fromEsp/control/device/2", "off");
          //sendDataToInternet("device2","off");
       }
       if(stateFromSystemToInternet[4] == '1' && stateFromSystemToInternet[4] != stateFromInternetToSystem[4]){
          stateFromInternetToSystem[4] = stateFromSystemToInternet[4];
          client.publish("fromEsp/control/device/3", "on");
          //sendDataToInternet("device3","on");
       }
       if(stateFromSystemToInternet[4] == '0' && stateFromSystemToInternet[4] != stateFromInternetToSystem[4]){
          stateFromInternetToSystem[4] = stateFromSystemToInternet[4];
          client.publish("fromEsp/control/device/3", "off");
          //sendDataToInternet("device3","off");
       }
       if(stateFromSystemToInternet[5] == '1' && stateFromSystemToInternet[5] != stateFromInternetToSystem[5]){
          stateFromInternetToSystem[5] = stateFromSystemToInternet[5];
          client.publish("fromEsp/control/device/4", "on");
          //sendDataToInternet("device4","on");
       }
       if(stateFromSystemToInternet[5] == '0' && stateFromSystemToInternet[5] != stateFromInternetToSystem[5]){
          stateFromInternetToSystem[5] = stateFromSystemToInternet[5];
          client.publish("fromEsp/control/device/4", "off");
          //sendDataToInternet("device4","off");
       }
       if(stateFromSystemToInternet[6] == '1' && stateFromSystemToInternet[6] != stateFromInternetToSystem[6]){
          stateFromInternetToSystem[6] = stateFromSystemToInternet[6];
          client.publish("fromEsp/control/device/5", "on");
          //sendDataToInternet("device5","on");
       }
       if(stateFromSystemToInternet[6] == '0' && stateFromSystemToInternet[6] != stateFromInternetToSystem[6]){
          stateFromInternetToSystem[6] = stateFromSystemToInternet[6];
          client.publish("fromEsp/control/device/5", "off");
          //sendDataToInternet("device5","off");
       }
       if(stateFromSystemToInternet[7] == '1' && stateFromSystemToInternet[7] != stateFromInternetToSystem[7]){
          stateFromInternetToSystem[7] = stateFromSystemToInternet[7];
          client.publish("fromEsp/control/device/6", "on");
          //sendDataToInternet("device6","on");
       }
       if(stateFromSystemToInternet[7] == '0' && stateFromSystemToInternet[7] != stateFromInternetToSystem[7]){
          stateFromInternetToSystem[7] = stateFromSystemToInternet[7];
          client.publish("fromEsp/control/device/5", "off");
          //sendDataToInternet("device5","off");
       }
       if(stateFromSystemToInternet[8] == '1' && stateFromSystemToInternet[8] != stateFromInternetToSystem[8]){
          stateFromInternetToSystem[8] = stateFromSystemToInternet[8];
          client.publish("fromEsp/control/device/7", "on");
          //sendDataToInternet("device7","on");
       }
       if(stateFromSystemToInternet[8] == '0' && stateFromSystemToInternet[8] != stateFromInternetToSystem[8]){
          stateFromInternetToSystem[8] = stateFromSystemToInternet[8];
          client.publish("fromEsp/control/device/7", "off");
          //sendDataToInternet("device7","off");
       }
    }
    /*
    if(stateFromSystemToInternet[2] == '3'){
        if(stateFromSystemToInternet[9] == 'T'){
          Temp = (stateFromSystemToInternet[7]-48)*10 + (stateFromSystemToInternet[8]-48);
          sendDataFromSensorToInternet("Temp",Temp);
        }
        if(stateFromSystemToInternet[9] == 'H'){
          Humid = (stateFromSystemToInternet[7]-48)*10 + (stateFromSystemToInternet[8]-48);
          sendDataFromSensorToInternet("Humid",Humid);
        }
        if(stateFromSystemToInternet[9] == 'M'){
          Human = (stateFromSystemToInternet[7]-48)*10 + (stateFromSystemToInternet[8]-48);
          sendDataFromSensorToInternet("Human",Human);
        }
        if(stateFromSystemToInternet[9] == 'G'){
          Gas = (stateFromSystemToInternet[7]-48)*10 + (stateFromSystemToInternet[8]-48);
          sendDataFromSensorToInternet("Gas",Gas);
        }
        if(stateFromSystemToInternet[9] == 'P'){
          Power = (stateFromSystemToInternet[6]-48)*100 + (stateFromSystemToInternet[7]-48)*10 + (stateFromSystemToInternet[8]-48);
          sendDataFromSensorToInternet("Power",Power);
        }
    }
    */
}
//--------------------------------------------------------------------------------------------------------------------


void loop() {
  client.loop();
  
  if(Serial.available() > 0){
     stateFromSystemToInternet = Serial.readString();
     //Serial.println("test: ");
     //Serial.println(stateFromSystemToInternet);
     processDataFromSystem();
  }
}
