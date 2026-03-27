#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include "DHT.h"

#define DHTPIN 2 
#define DHTTYPE DHT11
#define LDR_PIN A0

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(115200);  // Initialize serial
  checkWifiSetup();
    
  ThingSpeak.begin(client);  //Initialize ThingSpeak
  dht.begin(); //Initialize dht
}

void loop() {
  connectWifi();
  int light = readLight();
  float temp = readTemperature();
  
  printSensors(light, temp);

  // send to Thingspeak
  sendToThingSpeak(temp, light);

  delay(30000); // Wait 30 seconds to update the channel again
}

void checkWifiSetup()
{
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  
  String fv = WiFi.firmwareVersion();
  if (fv != "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connectWifi()
{
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
}

void printSensors(int light, float temp)
{
  Serial.print("Light: ");
  Serial.print(light);
  Serial.print(" | Temp: ");
  Serial.print(temp);
  Serial.println("°C ");
  
}

int readLight()
{
  return analogRead(LDR_PIN); 
}  

float readTemperature()
{
  float temp = dht.readTemperature();
  if (isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }
  return temp; 
}

void channelUpdated(int x)
{
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}

void sendToThingSpeak(float temp, int light)
{ 
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, light);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  channelUpdated(x);
}