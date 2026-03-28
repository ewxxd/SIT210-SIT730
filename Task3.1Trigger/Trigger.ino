#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include <PubSubClient.h> 

// WiFi Credentials
#define WIFI_SSID "VX420-EBC8"
#define WIFI_PASSWORD "325275A594ADF"

// MQTT Broker Credentials
#define MQTT_SERVER "2ac68edf2da14d5bb8f6a394c04c73ef.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_TOPIC "sensor"
#define MQTT_USER "hivemq.webclient.1774610845467"
#define MQTT_PASSWORD "aAqHSPB:4f3F&mi89@.b"

// Clients
WiFiSSLClient wifiSSLClient;
PubSubClient mqttClient(wifiSSLClient);
BH1750 lightSensor;

// MQTT Connection
void connectMQTT() {
   while (!mqttClient.connected()) {
       Serial.println("Connecting to MQTT...");
       if (mqttClient.connect("Nano33IoT_001", MQTT_USER, MQTT_PASSWORD)) {
           Serial.println("Connected to MQTT Broker!");
       } else {
           Serial.print("Failed, rc=");
           Serial.print(mqttClient.state());
           Serial.println(" Retrying in 5 seconds...");
           delay(5000);
       }
   }
}

// Sensor setup
void setupLightSensor() {
   Wire.begin();
   if (!lightSensor.begin()) {
       Serial.println("Error initializing BH1750 sensor!");
       while (1);
   }
   Serial.println("BH1750 Test");
}

// WiFi setup
void connectWifi() {
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
   while (WiFi.status() != WL_CONNECTED) {
       delay(1000);
       Serial.println("Connecting to WiFi...");
   }
   Serial.println("Connected to WiFi");
}

// MQTT loop maintenance
void mqttConnection() {
   if (!mqttClient.connected()) {
       connectMQTT();
   }
   mqttClient.loop();
}

// Sensor trigger logic
void lightSensorTrigger() {
   float lux = lightSensor.readLightLevel();

   Serial.print("Light Level: ");
   Serial.print(lux);
   Serial.println(" lx");

   if (lux > 100) {
       Serial.println("Light is bright!");
       mqttClient.publish(MQTT_TOPIC, "Light is bright!");
   } 
   else if (lux > 10) {
       Serial.println("Light is normal!");
       mqttClient.publish(MQTT_TOPIC, "Light is normal!");
   } 
   else {
       Serial.println("No light");
       mqttClient.publish(MQTT_TOPIC, "No light");
   }
}

void setup() {
   Serial.begin(115200);

   setupLightSensor();
   connectWifi();

   mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
   connectMQTT();
}

void loop() {
   mqttConnection();
   lightSensorTrigger();
   delay(5000);
}
