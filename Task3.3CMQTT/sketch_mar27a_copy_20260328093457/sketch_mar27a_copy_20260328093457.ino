#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi Credentials
#define WIFI_SSID "VX420-EBC8"
#define WIFI_PASSWORD "325275A594ADF"

// MQTT Broker Credentials
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883

// Clients
WiFiClient wifiClient;  
PubSubClient mqttClient(wifiClient);

//sensors and leds
const int hallwayLED = 5;
const int bathroomLED = 2;
const int trigPin = 8;
const int echoPin = 9;

//Thesholds
const int waveThreshold = 20;
const int patThreshold = 6;

bool lastWave = false;
bool lastPat = false;
unsigned long lastSensorRead = 0; // For non-blocking timer

// Handles LED Actuation
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  // Print the Payload 
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  if (strcmp(topic, "ES/Wave") == 0) {
    digitalWrite(hallwayLED, HIGH);
    digitalWrite(bathroomLED, HIGH);
    Serial.println("Lights ON");
  } 
  else if (strcmp(topic, "ES/Pat") == 0) {
    digitalWrite(hallwayLED, LOW);
    digitalWrite(bathroomLED, LOW);
    Serial.println("Lights OFF");
  }
}

//SENSOR LOGIC
void hcSensorTrigger() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  if (distance > 0 && distance < 100) { 
    Serial.print("Distance: "); 
    Serial.println(distance);
    
    if (distance < waveThreshold && distance > patThreshold) {
      if (!lastWave) {
        mqttClient.publish("ES/Wave", "Wave detected by Ewoud");
        lastWave = true;
        lastPat = false;
      }
    } 
    else if (distance <= patThreshold && distance > 0) {
      if (!lastPat) {
        mqttClient.publish("ES/Pat", "Pat detected by Ewoud");
        lastPat = true;
        lastWave = false;
      }
    }
    else {
      lastWave = false;
      lastPat = false;
    }
  }
}

//SETUP FUNCTIONS
void connectWifi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to HiveMQ...");
    if (mqttClient.connect("Ewoud_SIT210_Unique_99")) { 
      Serial.println("Connected to HiveMQ Broker!");
      mqttClient.subscribe("ES/Wave");
      mqttClient.subscribe("ES/Pat");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(hallwayLED, OUTPUT);
  pinMode(bathroomLED, OUTPUT);

  connectWifi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  connectMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  if (millis() - lastSensorRead > 300) {
    hcSensorTrigger();
    lastSensorRead = millis();
  } 
}