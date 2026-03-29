#include <Wire.h>
#include <BH1750.h>

const int pirPin = 2;        // PIR sensor
const int switchPin = 3;     // Slider switch
const int porchLight = 4;    // LED 1
const int hallwayLight = 5;  // LED 2

BH1750 lightMeter;

// Switch interrupt
volatile bool switchTriggered = false;
volatile unsigned long lastSwitchTime = 0;

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

  attachInterrupt(digitalPinToInterrupt(switchPin), switchISR, FALLING);

  Serial.println("System ready");
}

void loop() {
  // Pass the light reading into the logic functions
  float lux = lightMeter.readLightLevel();
  bool isDark = (lux == 0);

  motionLogic();
  lightsLogic(isDark); // Pass isDark as a parameter

  delay(50); 
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
  
  if (autoLightsOn) {
    currentStatus = "Dark & Motion detected - Lights On";
  } else if (!isDark) {
    currentStatus = "Not dark - lights off";
  } else if (!motionDetected) {
    currentStatus = "No motion - lights off";
  }

  // Only print if the status has changed AND we aren't in manual mode
  if (currentStatus != lastStatus && !lightsOnBySwitch) {
    Serial.println(currentStatus);
    lastStatus = currentStatus;
  }
}

void motionLogic() {
  int pirState = digitalRead(pirPin);
  
  // Update Motion Logic
  if (pirState == HIGH) {
    motionDetected = true;
    motionTimer = millis(); 
  }

  if (motionDetected && (millis() - motionTimer > motionTimeout)) {
    motionDetected = false;
  }

  // Handle Manual Switch Interrupt
  if (switchTriggered) {
    switchTriggered = false;
    lightsOnBySwitch = !lightsOnBySwitch;
    Serial.println(lightsOnBySwitch ? "Manual Override: Lights ON" : "Manual Override: Lights OFF");
    
    // Clear lastStatus so the auto-logic prints again when Linda turns manual OFF
    if (!lightsOnBySwitch) lastStatus = ""; 
  }
}

void switchISR() {
  unsigned long now = millis();
  if (now - lastSwitchTime > 200) { 
    switchTriggered = true;
    lastSwitchTime = now;
  }
}