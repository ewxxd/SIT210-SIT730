#include <Servo.h>

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
  
  gateServo.attach(servoPin);
  gateClose();
}

void loop() {
  //LISTEN FOR TELEGRAM COMMAND
  telegramCommand();

  //MOTION DETECTION
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

  //MOTION
  if (distance > 0 && distance < 20) {
    if (!waitingForAuth && !gateIsCurrentlyOpen) {
      Serial.println("MOTION_DETECTED");
      waitingForAuth = true; 
    }
  }

  //PATH IS CLEAR
  if (gateIsCurrentlyOpen) {
    if (distance > 35 || distance == 0) { 
      delay(2000); 
      gateClose();
      gateIsCurrentlyOpen = false;
      waitingForAuth = false; 
    }
  }
}

void telegramCommand() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    data.trim();
    if (data == "Y" || data == "y") {
       gateOpen();
       gateIsCurrentlyOpen = true;
       waitingForAuth = false; 
    }
  }
}