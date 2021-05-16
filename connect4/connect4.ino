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


// lcd screen defines with instance of the library
#define cs 10
#define dc 9
#define rst 8
#define width TFTscreen.width()
#define height TFTscreen.height()
TFT TFTscreen = TFT(cs, dc, rst);

// rectangle variables
#define LEFT_CORNER_X 20
#define LEFT_CORNER_Y 30
#define RECT_WIDTH (width - (LEFT_CORNER_X * 2))
#define RECT_HEIGHT (height - (LEFT_CORNER_Y * 2) + 10)
#define INTERVAL_HEIGHT (RECT_HEIGHT/6)
#define INTERVAL_WIDTH (RECT_WIDTH/7)

//game variables
#define GAME_INIT 0
#define GAME_BEGINS 1
#define GAME_STARTED 2
volatile byte game_state;
byte player_turn;

//setup the screen
void setup_lcd_screen() {
  //initialize the library
  TFTscreen.begin();

  // clear the screen
  TFTscreen.background(0, 0, 0);
  //set the text size
  TFTscreen.setTextSize(2);
}


void setup() {
  // debug serial print
  Serial.begin(9600);

  // set interrupts for control buttons using PinChangeInt library

  // left button interrupt
  //pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);

  // center button interrupt -> the only one used in game setup
  //pinMode(CENTER_BUTTON_PIN, INPUT_PULLUP);
  PCintPort::attachInterrupt(CENTER_BUTTON_PIN, game_setup_interrupt, RISING);

  // right button interrupt
  //pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);

  // set pins 2,3,4 as input
  DDRD = DDRD | B11100011;

  //setup screen
  setup_lcd_screen();

  //game state
  game_state = GAME_INIT;
  player_turn = 0;
}


void draw_lines() {
  for(int i = 1 ; i <= 5 ; i++) {
    TFTscreen.line(LEFT_CORNER_X, LEFT_CORNER_Y + INTERVAL_HEIGHT * i, LEFT_CORNER_X + RECT_WIDTH, LEFT_CORNER_Y + INTERVAL_HEIGHT * i);
  }

  for(int i = 1 ; i <= 6 ; i++) {
    TFTscreen.line(LEFT_CORNER_X + INTERVAL_WIDTH * i, LEFT_CORNER_Y, LEFT_CORNER_X + INTERVAL_WIDTH * i, LEFT_CORNER_Y + RECT_HEIGHT);
  }
}

void loop() {
  // set a font color
  TFTscreen.stroke(0, 0, 255);

  if (game_state == GAME_INIT) {
    TFTscreen.text("Press green", 15, 50);
    TFTscreen.text("to play", 40, 80);
  } else if (game_state == GAME_BEGINS) {
    TFTscreen.background(0, 0, 0);
    TFTscreen.rect(20, 30, RECT_WIDTH, RECT_HEIGHT);
    TFTscreen.circle(0, 0, 2);
    draw_lines();
    game_state = GAME_STARTED;
    PCintPort::detachInterrupt(CENTER_BUTTON_PIN);
    PCintPort::attachInterrupt(CENTER_BUTTON_PIN, control_buttons_interrupt, RISING);
    PCintPort::attachInterrupt(RIGHT_BUTTON_PIN, control_buttons_interrupt, RISING);
    PCintPort::attachInterrupt(LEFT_BUTTON_PIN, control_buttons_interrupt, RISING);
  }
}


void control_buttons_interrupt() {
  if ((PIND & (1 << PD4)) != 0) {
      if(millis() - right_button_last_debounce_time >= 150) {
        right_button_last_debounce_time = millis();
        Serial.println(4);
      }
  } else if ((PIND & (1 << PD3)) != 0) {
      if(millis() - center_button_last_debounce_time >= 150) {
        center_button_last_debounce_time = millis();
        Serial.println(3);
      }
  } else if ((PIND & (1 << PD2)) != 0) {
      if(millis() - left_button_last_debounce_time >= 150) {
        left_button_last_debounce_time = millis();
        Serial.println(2);
      }
  }
}

void game_setup_interrupt() {
  if ((PIND & (1 << PD3)) != 0) {
      if(millis() - center_button_last_debounce_time >= 150) {
        center_button_last_debounce_time = millis();
        if (game_state == GAME_INIT) {
            game_state = GAME_BEGINS;
        }
      }
  }
}
