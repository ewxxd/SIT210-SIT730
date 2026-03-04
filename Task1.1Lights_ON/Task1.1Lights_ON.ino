#define LED_PORCH 8
#define LED_HALLWAY 7
#define BUTTON_PIN 5

const unsigned long timerThirtySec = 30000;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_PORCH, OUTPUT);
  pinMode(LED_HALLWAY, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
}

// the loop function runs over and over again forever
void loop() {

  if (buttonPressed())
    turnOnLights();
  
}

bool buttonPressed(){
  return digitalRead(BUTTON_PIN) == HIGH;
}

void turnOnLights(){
  digitalWrite(LED_PORCH, HIGH);
  digitalWrite(LED_HALLWAY, HIGH);
  delay(timerThirtySec);
  digitalWrite(LED_PORCH, LOW);
  delay(timerThirtySec);
  digitalWrite(LED_HALLWAY, LOW); 
}
