#include <Servo.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi Credentials
#define WIFI_SSID "EwoudHotspot"
#define WIFI_PASSWORD "abcdefgh"

// MQTT Broker Credentials
#define MQTT_SERVER "2ac68edf2da14d5bb8f6a394c04c73ef.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_TOPIC "Motion"
#define MQTT_USER "hivemq.webclient.1774610845467"
#define MQTT_PASSWORD "aAqHSPB:4f3F&mi89@.b"

// Clients
WiFiSSLClient wifiClient;
PubSubClient mqttClient(wifiClient);

Servo gateServo;
const int ledOpen = 3;
const int ledClose = 4;
const int trigPin = 7;
const int echoPin = 6;
const int servoPin = 9;

bool waitingForAuth = false; 
bool gateIsCurrentlyOpen = false;

void setup() {
  Serial.begin(9600);
  pinMode(ledOpen, OUTPUT);
  pinMode(ledClose, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  connectWifi();
  mqttClient.setBufferSize(512);
  mqttClient.setSocketTimeout(30);
  mqttClient.setKeepAlive(60);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(telegramResponse);
  connectMQTT();

  gateServo.attach(servoPin);
  gateClose();
}

void loop() {  
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();
  motionDetected();
  delay(100);
}

void gateOpen() {
  digitalWrite(ledOpen, HIGH);
  digitalWrite(ledClose, LOW);
  gateServo.write(180);
}

void gateClose() {
  digitalWrite(ledOpen, LOW);
  digitalWrite(ledClose, HIGH);
  gateServo.write(90);
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  if (duration == 0) return 1000;
  return duration * 0.034 / 2;
}

void motionDetected() {
  int distance = getDistance();

  if (distance > 0 && distance < 20) {
    if (!waitingForAuth && !gateIsCurrentlyOpen) {
      Serial.println("Motion detected, publishing...");
      bool published = mqttClient.publish("ES/Motion", "Motion Detected.");
      if (published) {
        Serial.println("Published successfully.");
        waitingForAuth = true;
      } else {
        Serial.println("Publish failed.");
      }
    }
  }

  if (gateIsCurrentlyOpen) {
    if (distance > 35 || distance == 0) { 
      delay(2000); 
      gateClose();
      gateIsCurrentlyOpen = false;
      waitingForAuth = false; 
    }
  }
}

void telegramResponse(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (message == "Y" || message == "y") {
    Serial.println("Opening gate...");
    gateOpen();
    gateIsCurrentlyOpen = true;
    waitingForAuth = false; 
  } else if (message == "N" || message == "n") {
    Serial.println("Closing gate...");
    gateClose();
    gateIsCurrentlyOpen = false;
    waitingForAuth = false; 
  } else {
    Serial.println("Message not recognised.");
  }
}

void connectWifi() {
  Serial.println("Starting WiFi connection...");
  int status = WL_IDLE_STATUS;

  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("WiFi Status: ");
    Serial.println(status);
    delay(5000);
  }

  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("Nano33IoT_001", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT Broker!");
      mqttClient.subscribe("ES/Auth");
      
      if (waitingForAuth) {
        Serial.println("Republishing motion after reconnect...");
        mqttClient.publish("ES/Motion", "Motion Detected.");
      }
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
