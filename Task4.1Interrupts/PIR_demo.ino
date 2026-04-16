#include <Wire.h>
#include <BH1750.h>

const int pirPin = 2;        // PIR sensor
const int switchPin = 3;     // Slider switch
const int porchLight = 4;    // LED 1
const int hallwayLight = 5;  // LED 2

BH1750 lightMeter;

// Switch interrupt
volatile bool switchTriggered = false;
volatile bool pirTriggered = false;
volatile unsigned long lastSwitchTime = 0;
volatile unsigned long lastPirTime = 0;
const unsigned long debounceDelay = 250;

// Persistent states
bool lightsOnBySwitch = false;
bool motionDetected = false;
unsigned long motionTimer = 0;
const unsigned long motionTimeout = 5000; 

// Tracking for Serial Printing
String lastStatus = ""; 

void setup() {
  Serial.begin(115200);
  Wire.begin();
  lightMeter.begin();

  pinMode(porchLight, OUTPUT);
  pinMode(hallwayLight, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(switchPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(pirPin), pirISR, RISING);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchISR, FALLING);

  Serial.println("System ready");
}

void loop() {
  processInputs();
}

void lightsLogic(bool isDark) {
  //Determine if automatic conditions are met
  bool autoLightsOn = (isDark && motionDetected);
  
  // Determine if lights should be ON (Manual override OR Auto)
  bool lightsShouldBeOn = lightsOnBySwitch || autoLightsOn;

  // Update LEDs
  digitalWrite(porchLight, lightsShouldBeOn ? HIGH : LOW);
  digitalWrite(hallwayLight, lightsShouldBeOn ? HIGH : LOW);

  // Handle Specific Print Requirements
  String currentStatus = "";
  
  if (lightsOnBySwitch) {
    currentStatus = "Switch - lights on";
  } else if (autoLightsOn) {
    currentStatus = "Dark & Motion - Lights On";
  } else {
    currentStatus = isDark ? "Dark & No Motion - Lights Off" : "Not Dark - Lights Off";
  }

  // Only print if the status has changed AND we aren't in manual mode
  if (currentStatus != lastStatus) {
    Serial.println(currentStatus);
    lastStatus = currentStatus;
  }
}

void processInputs(){
  unsigned long now = millis();
  float lux = lightMeter.readLightLevel();
  bool isDark = (lux < 20);

  if (pirTriggered && (now - lastPirTime > debounceDelay)){
    pirTriggered = false;
    lastPirTime = now;
    motionLogic();
  }

  if (switchTriggered && (now - lastSwitchTime > debounceDelay)){
    SwitchTriggered = false;
    lastSwitchTime = now;
    switchLogic();
  }

  if (motionDetected && (now - motionTimer > motionTimeout)){
    motionDeected = false;
  }

  ligthsLogic(isDark);
  delay(50);
}

void motionLogic() {
    motionDetected = true;
    motionTimer = millis(); 
    Serial.println("Motion detected");  
}

void switchLogic() {
    lightsOnBySwitch = !lightsOnBySwitch;
    Serial.print("Manual Switch: ");
    Serial.println(lightsOnBySwitch ? "ON" : "OFF");
}

void pirISR() {
  pirTriggered = true;
}

void switchISR() {
  switchTriggered = true;
}
