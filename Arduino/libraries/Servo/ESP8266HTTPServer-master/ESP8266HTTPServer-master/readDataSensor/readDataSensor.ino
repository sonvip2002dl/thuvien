
//@Author: EPCB TECH
//@Date: 2020
//@Description: ESP8266 With ThingSpeak Server

#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

const char ssid[]		= "SSID";  		// your network SSID (name)
const char password[]	= "PASSWORD";   // your network password         
WiFiClient  client;

//---------Channel Details---------//
unsigned long counterChannelNumber	= 12345678;         // Channel ID
const char * myCounterReadAPIKey 	= "READ API KEY"; 	// Read API Key
const int FieldNumber1 				= 1;  				// The field you wish to read
const int FieldNumber2 				= 2;
const int FieldNumber3 				= 3;  

float statusCode;

void setup()
{
  Serial.begin(115200);
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
    }
  Serial.println("\r\nWiFi connected");
  logoConsole();
  ThingSpeak.begin(client);
}

void loop()
{
	readData();
}

void readData(){
	//---------------- Channel 1 ----------------//
  float temp	= ThingSpeak.readLongField(counterChannelNumber, FieldNumber1, myCounterReadAPIKey);
  statusCode	= ThingSpeak.getLastReadStatus();
  if (statusCode == 200)
  {
     Serial.print("Temp: ");
     Serial.print(temp);
     Serial.print("°C");
     Serial.print("\n");
  }
  else
  {
    Serial.println("Unable to read channel / No internet connection");
  }
  delay(50);

  //---------------- Channel 2 ----------------//
  float humidity 	= ThingSpeak.readLongField(counterChannelNumber, FieldNumber2, myCounterReadAPIKey);
  statusCode 		= ThingSpeak.getLastReadStatus();
  if (statusCode == 200)
  {
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%");
    Serial.print("\n");
  }
  else
  {
    Serial.println("Unable to read channel / No internet connection");
  }
  delay(100);
  
  //---------------- Channel 3 ----------------//
  float moist	= ThingSpeak.readLongField(counterChannelNumber, FieldNumber2, myCounterReadAPIKey);
  statusCode	= ThingSpeak.getLastReadStatus();
  if (statusCode == 200)
  {
    Serial.print("Soil Moist: ");
    Serial.print(moist);
    Serial.print("%");
    Serial.print("\n");
  }
  else
  {
    Serial.println("Unable to read channel / No internet connection");
  }
  delay(100);
}

void logoConsole(void) {
  Serial.printf("----------------------\\\\\n");
  Serial.printf("|| EPCBTechEPCBTechEPCB ||\n");
  Serial.printf("|| P                  T ||\n");
  Serial.printf("|| C      .*.         e ||\n");
  Serial.printf("|| B      * *  .*.    c ||\n");
  Serial.printf("|| T***.  * *  * `****h ||\n");
  Serial.printf("|| e   `**' *  *      E ||\n");
  Serial.printf("|| c        `**'      P ||\n");
  Serial.printf("|| h                  C ||\n");
  Serial.printf("|| EPCBTechEPCBTechEPCB ||\n");
  Serial.printf("----------------------//\n");
  Serial.printf("//---------------------------------------------------------------------------------\\\\\n");
  Serial.printf("||   __ _                            _   _  _             _ _                          ||\n");
  Serial.printf("||  / __| |_   _ _ _ _  _ _   ___| | |  \\/  | __  _ _ () |_ _  _ __                   ||\n");
  Serial.printf("|| | |   | '_ \\ / ` | ' \\| '_ \\ / _ \\ | | |\\/| |/ _ \\| '_ \\| | __/ _ \\| '__|   ||\n");
  Serial.printf("|| | |___| | | | (| | | | | | | |  __/ | | |  | | () | | | | | || (_) | | \            ||\n");
  Serial.printf("||  \\____|_| |_|\\__,_|_| |_|_| |_|\\___|_| |_|  |_|\\___/|_| |_|_|\\__\\___/|_|      ||\n");
  Serial.printf("\\\\---------------------------------------------------------------------------------//  \n");
}

