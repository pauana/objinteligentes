/*
  WriteSingleField
  
  Description: Writes a value to a channel on ThingSpeak every 20 seconds.
  
  Hardware: ESP32 based boards
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires installation of EPS32 core. See https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md for details. 
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2020, The MathWorks, Inc.
*/

#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

int number = 0;
long valorMedio;
void setup() {

  valorMedio = 0;
  long med = 0;
  for(int t=0; t<10000; t++) {
    int flexSensorReading = analogRead(34);
    med = (med + flexSensorReading) / 2;
  }
  valorMedio = med;
  Serial.println(valorMedio);

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  delay(200);
  digitalWrite(2, LOW);
  
  Serial.begin(115200);  //Initialize serial
  
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  long med = 0;
  long desvio = 0;
  for(int t=0; t<1000; t++) {
    int flexSensorReading = analogRead(34);
    med = (med + flexSensorReading) / 2;
    if(med!=valorMedio) {
      desvio++;
    }
  }
  //Serial.println(desvio);
  long flex0toX = map(desvio, 0, 1000, 0, 15);
  Serial.println(flex0toX);

  if(flex0toX>14) {
    int tries = 0;
    int x = 0;
    while ((x != 200) && (tries < 10))
    {
      x = ThingSpeak.writeField(myChannelNumber, 1, flex0toX, myWriteAPIKey);
      if(x == 200){
        Serial.println("Channel update successful.");
      }
      else{
        Serial.println("Problem updating channel. HTTP error code " + String(x));
        delay(250);
      }
      tries ++;
    }
    
    digitalWrite(2, HIGH);
    delay(200);  
    digitalWrite(2,LOW);
  }
}