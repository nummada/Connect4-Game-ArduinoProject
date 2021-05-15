#include "buttons.h"
#include <TFT.h>  
#include <SPI.h>

#define cs   10
#define dc   9
#define rst  8

TFT TFTscreen = TFT(cs, dc, rst);

const byte interruptPin = 2;
volatile byte state = LOW;

//buttons
Button left_button;
Button center_button;
Button right_button;

void setup() {

  //init buttons
  left_button.init_button(3);
  center_button.init_button(4);
  right_button.init_button(2);
  
  // set buttons as inputs
  pinMode(left_button.pin, INPUT);
  pinMode(center_button.pin, INPUT);
  pinMode(right_button.pin, INPUT);

  // debug print
  Serial.begin(9600);

  //lcd screen
  //initialize the library
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);
  //set the text size
  TFTscreen.setTextSize(2);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);
}


void loop() {
  //left_button.toggle_button();
  //center_button.toggle_button();
  //right_button.toggle_button();

  
  // set a random font color
  TFTscreen.stroke(random(0, 255),random(0, 255),random(0, 255));
  
  // print Hello, World! in the middle of the screen
  TFTscreen.text("Hello, World!", 6, 57);
  
  // wait 200 miliseconds until change to next color

  
}

void blink() {
  Serial.print(digitalRead(2));
  Serial.println(random(0, 255));
}
