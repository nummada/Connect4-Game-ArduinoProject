#include <Arduino.h>
#include "buttons.h"

void Button::init_button(int pin){
  this->pin = pin;
  this->last_state = LOW;
  this->last_debounce_time = 0;
}

void Button::toggle_button() {
  int reading = digitalRead(this->pin);

  if (reading != this->last_state) {
    this->last_debounce_time = millis();
  }

  if ((millis() - this->last_debounce_time) > 50) {
    if (reading != this->state) {
      this->state = reading;
      if (this->state == HIGH) {
        Serial.println(this->pin, DEC);
      }
    }    
  }
  this->last_state = reading;
}
