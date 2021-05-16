// library used for interrupts
#include "PinChangeInt.h"

// library used for lcd screen
#include <TFT.h>  
#include <SPI.h>

// defines and variables for control pins
#define LEFT_BUTTON_PIN 2
unsigned long left_button_last_debounce_time;
#define CENTER_BUTTON_PIN 3
unsigned long center_button_last_debounce_time;
#define RIGHT_BUTTON_PIN 4
unsigned long right_button_last_debounce_time;


// lcd screen init with instance of the library
#define cs   10
#define dc   9
#define rst  8
TFT TFTscreen = TFT(cs, dc, rst);



void setup_lcd_screen() {
  //initialize the library
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);
  //set the text size
  TFTscreen.setTextSize(2);
}


void setup() {
  // debug serial print
  Serial.begin(9600);

  // set interrupts for control buttons using PinChangeInt library

  // left button interrupt
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  PCintPort::attachInterrupt(LEFT_BUTTON_PIN, left_button_trigger, RISING);

  // center button interrupt
  pinMode(CENTER_BUTTON_PIN, INPUT_PULLUP);
  PCintPort::attachInterrupt(CENTER_BUTTON_PIN, center_button_trigger, RISING);

  // right button interrupt
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
  PCintPort::attachInterrupt(RIGHT_BUTTON_PIN, right_button_trigger, RISING);

  //setup screen
  setup_lcd_screen();
}


void loop() {
  //generate a random color
  int redRandom = random(0, 255);
  int greenRandom = random (0, 255);
  int blueRandom = random (0, 255);
  
  // set a random font color
  TFTscreen.stroke(redRandom, greenRandom, blueRandom);
  
  // print Hello, World! in the middle of the screen
  TFTscreen.text("Hello, World!", 6, 57);
  
  // wait 200 miliseconds until change to next color
  delay(200);
}

void button_intrerrupt(int pin) {
  if ((PIND & (1 << PD4)) != 0) {
    
      Serial.println("il vede pe 4");
  } else if ((PIND & (1 << PD3)) != 0) {
      Serial.println("il vede pe 3");
  } else if ((PIND & (1 << PD2)) != 0) {
      Serial.println("il vede pe 2");
  }
//  Serial.println(pin);
}

void right_button_trigger() {
  if((long)(millis() - right_button_last_debounce_time) >= 150) {
    button_intrerrupt(RIGHT_BUTTON_PIN);
    right_button_last_debounce_time = millis();
  }
}

void left_button_trigger() {
  if((long)(millis() - left_button_last_debounce_time) >= 150) {
    button_intrerrupt(LEFT_BUTTON_PIN);
    left_button_last_debounce_time = millis();
  }
}

void center_button_trigger() {
  if((long)(millis() - center_button_last_debounce_time) >= 150) {
    button_intrerrupt(CENTER_BUTTON_PIN);
    center_button_last_debounce_time = millis();
  }
}
