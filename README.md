# SIT210-SIT730
Task1.1 Lights ON

In this task, I used the Arduino IDE with an Arduino Uno R3 to create a light system for Linda. I created a physical circuit with one button and two LEDs with resistors. The goal was to create two lights that switch on when she presses the button, then the porch light goes off after 30 seconds and the hallway light goes off after 60 seconds.

I defined my pins and created a constant 30-second timer. I then initialized all my pins in the setup() method. I created several helper functions, buttonPressed() returns a boolean indicating if the button is pressed, turnOnLights() turns on both lights, turnOffLight(int ledName) turns off a specified light, and turnOnOneLight(int ledName) turns on a specified light.

Finally, in the main loop(), I check if the button is pressed via the buttonPressed() function, and if it is pressed, it calls the functions to execute the lighting sequence using the modular functions.
