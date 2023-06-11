#include "BluetoothSerial.h"
#include <WiFi.h>
#include "ThingSpeak.h"
#include <HardwareSerial.h>

//SoftwareSerial Serial2(D6, D7);
BluetoothSerial SerialBT;
WiFiClient client;

unsigned long myChannelNumber = 1832142;
const char * myWriteAPIKey = "RIDLB23SWWTMWBES";

String theft_flag = "";
String button_flag = "";

String myString = "";

char ssid[20];
char password[20];

String ssid_tmp = "";
String pass_tmp = "";
String Detection_flag = "";

int current_mode = 0;
int SerialCheck();
void Set_ssid();
void Set_Password();
void Wait_Detection();
void Connect_wifi();

void setup() {

  Serial.begin(115200);
  Serial2.begin(115200);   //Mega - ESP32
  SerialBT.begin("ESP32"); //Bluetooth device 이름

  Serial.println("The device started, now you can pair it with bluetooth!");
  //Serial2.begin(115200);

}



void loop() {
  if (current_mode == 0) {
    current_mode = SerialCheck();
  }
  else if (current_mode == 1) {
    Set_ssid();
  }
  else if (current_mode == 2) {
    Set_Password();
  }
  else if (current_mode == 3) {
    Connect_wifi();
  }
  else if (current_mode == 4) {
    Wait_Detection();
  }
  else if (current_mode == 5 ) {
    Detection_State();
  }
}

int SerialCheck() {
  int _mode = 0;
  if (SerialBT.available()) {
    String mode_tmp = SerialBT.readString();
    if (mode_tmp == "Set_SSID") {
      _mode = 1;
    }
    else if (mode_tmp == "Set_PassWord") {
      _mode = 2;
    }
    else if (mode_tmp == "Connect_wifi") {
      _mode = 3;
    }
  }
  return _mode;
}

void Set_ssid() {
  if (SerialBT.available()) {
    ssid_tmp = SerialBT.readString();
    ssid_tmp.toCharArray(ssid, ssid_tmp.length() + 1);
    SerialBT.print("Set SSID : " + ssid_tmp);
    current_mode = 0;
    delay(5);
  }
}

void Set_Password() {
  if (SerialBT.available()) {
    pass_tmp = SerialBT.readString();
    pass_tmp.toCharArray(password, pass_tmp.length() + 1);
    SerialBT.print("Set Password " + pass_tmp);
    current_mode = 0;
    delay(5);
  }
}

void Connect_wifi() {
  int count = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    if (count++ > 30) { // 15초
      current_mode = 0;
      SerialBT.print("Failed_Connection");
      return;
    }
    delay(500);
  }
  SerialBT.print("WifiConnected");
  Serial2.print("WifiConnected");

  ThingSpeak.begin(client);
  current_mode = 4;
}

void Wait_Detection() {
  if (SerialBT.available()) {
    Detection_flag = SerialBT.readString();
    if (Detection_flag == "Detecting_Start") {
      SerialBT.print(Detection_flag);
      Serial2.print("Detection_start");
      current_mode = 5;
      delay(5);
    }
  }
}

void Detection_State() {
  if (Serial2.available()) {
    theft_flag = Serial2.readString();
    SerialBT.print(theft_flag);
    Serial.print(theft_flag);
  }
  if (SerialBT.available()){
    Detection_flag = SerialBT.readString();
    if (Detection_flag == "Reset") {
      SerialBT.print(Detection_flag);
      Serial2.print("Reset");
      current_mode = 4;
      delay(5);
    }
  }
  if (theft_flag == "t") {
    SerialBT.println("check");
    ThingSpeak.setField(1, 1);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    
    Serial.println("Waiting 20 secs");

    theft_flag = "";
  }
}
