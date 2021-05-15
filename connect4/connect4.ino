#include <Arduino.h>
//buttons' pins
const int centerButton = 4;
const int leftButton = 3;
const int rightButton = 2;

// the current state of the center button
int centerButtonState;
// the current state of the left button
int leftButtonState;
// the current state of the right button
int rightButtonState;


// the previous state of center button
int lastCenterButtonState = LOW;
// the previous state of left button
int lastLeftButtonState = LOW;
// the previous state of right button
int lastRightButtonState = LOW;

// the last time the left button was toggled
unsigned long lastCenterDebounceTime = 0;
// the last time the left button was toggled
unsigned long lastLeftDebounceTime = 0;
// the last time the right button was toggled
unsigned long lastRightDebounceTime = 0;
// delay for debounce
unsigned long debounceDelay = 50;

void setup() {
  // set buttons as inputs
  pinMode(centerButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(rightButton, INPUT);

  // debug print
  Serial.begin(9600);
}

void toggle_left_button() {
  int reading = digitalRead(leftButton);

  if (reading != lastLeftButtonState) {
    lastLeftDebounceTime = millis();
  }

  if ((millis() - lastLeftDebounceTime) > debounceDelay) {
    if (reading != leftButtonState) {
      leftButtonState = reading;
      if (leftButtonState == HIGH) {
        Serial.print("left button is on\n");
      }
    }    
  }
  lastLeftButtonState = reading;
}

void toggle_center_button() {
  int reading = digitalRead(centerButton);

  if (reading != lastCenterButtonState) {
    lastCenterDebounceTime = millis();
  }

  if ((millis() - lastCenterDebounceTime) > debounceDelay) {
    if (reading != centerButtonState) {
      centerButtonState = reading;
      if (centerButtonState == HIGH) {
        Serial.print("center button is on\n");
      }
    }    
  }
  lastCenterButtonState = reading;
}

void loop() {
  toggle_center_button();
  toggle_left_button();
  

  int readingRight = digitalRead(rightButton);
  if (readingRight != lastRightButtonState) {
    lastRightDebounceTime = millis();
  }

  if ((millis() - lastRightDebounceTime) > debounceDelay) {
    if (readingRight != rightButtonState) {
      rightButtonState = readingRight;
      if (rightButtonState == HIGH) {
        Serial.print("right button is on\n");
      }
    }
  }
  lastRightButtonState = readingRight;
}
