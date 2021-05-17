// library used for interrupts
#include "PinChangeInt.h"
// library used for lcd screen
#include <TFT.h>
#include <SPI.h>

// defines and variables for control pins
#define LEFT_BUTTON 2
unsigned long left_button_last_debounce_time;
#define CENTER_BUTTON 3
unsigned long center_button_last_debounce_time;
#define RIGHT_BUTTON 4
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
#define INTERVAL_HEIGHT (RECT_HEIGHT / 6)
#define INTERVAL_WIDTH (RECT_WIDTH / 7)

//game variables
#define GAME_INIT 0
#define GAME_BEGINS 1
#define GAME_STARTED 2
volatile byte game_state;
byte player_switched;
char *player_turn;

//design defines
#define PLAYER_TURN_TEXT_X_POS 75
#define red_background 0
#define green_background 0
#define blue_background 0
#define red_font 0
#define green_font 0
#define blue_font 255

//select circle defines
#define SELECTOR_START_X (width / 2)
#define SELECTOR_START_Y (LEFT_CORNER_Y - 7)
#define CIRCLE_START_POSITION 0
#define STANDING 0
signed char circle_position;
signed char circle_state;

//game table
byte game_table[6][7];

//setup the screen
void setup_lcd_screen() {
  //initialize the library
  TFTscreen.begin();

  // clear the screen
  TFTscreen.background(red_background, green_background, blue_background);
  //set the text size
  TFTscreen.setTextSize(2);
}

void set_color_by_player() {
  if (player_turn[0] == '0') {
    TFTscreen.stroke(0, 0, 255);
  } else {
    TFTscreen.stroke(0, 255, 255);
  }
}

void draw_text_by_player(char *text, byte x_pos, byte y_pos) {
  set_color_by_player();
  TFTscreen.text(text, x_pos, y_pos);
}

void clear_and_reinit_selector() {
  TFTscreen.stroke(blue_background, green_background, red_background);
  TFTscreen.circle(SELECTOR_START_X + (circle_position * INTERVAL_WIDTH), SELECTOR_START_Y, 5);

  set_color_by_player();
  
  TFTscreen.circle(SELECTOR_START_X , SELECTOR_START_Y, 5);
  
  circle_position = CIRCLE_START_POSITION;
  circle_state = STANDING;
}


void setup() {
  // debug serial print
  Serial.begin(9600);

  // center button interrupt -> the only one used in game setup
  PCintPort::attachInterrupt(CENTER_BUTTON, game_setup_interrupt, RISING);

  // set pins 2,3,4 as input
  DDRD = DDRD | B11100011;

  //setup screen
  setup_lcd_screen();

  //game state
  game_state = GAME_INIT;
  player_turn = "0";
  player_switched = 0;

  //setup circle
  circle_position = CIRCLE_START_POSITION;
  circle_state = STANDING;
}


void draw_lines() {
  for (int i = 1 ; i <= 5 ; i++) {
    TFTscreen.line(LEFT_CORNER_X, LEFT_CORNER_Y + INTERVAL_HEIGHT * i, LEFT_CORNER_X + RECT_WIDTH, LEFT_CORNER_Y + INTERVAL_HEIGHT * i);
  }

  for (int i = 1 ; i <= 6 ; i++) {
    TFTscreen.line(LEFT_CORNER_X + INTERVAL_WIDTH * i, LEFT_CORNER_Y, LEFT_CORNER_X + INTERVAL_WIDTH * i, LEFT_CORNER_Y + RECT_HEIGHT);
  }
}

void clear_text_and_set_color(byte blue, byte green, byte red, char* text, byte x_pos, byte y_pos) {
  TFTscreen.stroke(blue_background, green_background, red_background);
  TFTscreen.text(text, x_pos, y_pos);
  TFTscreen.stroke(blue, green, red);
}

void clear_circle_and_set_color(byte blue, byte green, byte red, byte x_pos, byte y_pos, byte radius) {
  TFTscreen.stroke(blue_background, green_background, red_background);
  TFTscreen.circle(x_pos, y_pos, radius);
  TFTscreen.stroke(blue, green, red);
}

void move_circle() {
  if(circle_position + circle_state >= -3 && circle_position + circle_state <= 3) {
      
      clear_circle_and_set_color(blue_font,
                                green_font,
                                red_font,
                                SELECTOR_START_X + (circle_position * INTERVAL_WIDTH),
                                SELECTOR_START_Y,
                                5);

      // adds -1 (left) or 1 (right)
      circle_position += circle_state;
      set_color_by_player();
      // adds circle_position * INTERVAL_WIDTH to x (related to first position which is 0)
      TFTscreen.circle(SELECTOR_START_X + (circle_position * INTERVAL_WIDTH), SELECTOR_START_Y, 5);
    }
}

void occupy_element (int i, int j) {
  game_table[i][j] = 1;
  // draw the correspondent circle
  TFTscreen.circle(SELECTOR_START_X - INTERVAL_WIDTH * (3 - (circle_position + 3)),
                  SELECTOR_START_Y + INTERVAL_HEIGHT * (i + 1),
                  4);
  //change player's turn
  player_turn[0] ^= 1;
  //print the player's turn in the top left corner of screen
  draw_text_by_player(player_turn, PLAYER_TURN_TEXT_X_POS, 0);
  //finish the switch
  player_switched = 0;
  //reinitiate the circle selector
  clear_and_reinit_selector();
}

void init_interrupts_for_game_start() {
  PCintPort::detachInterrupt(CENTER_BUTTON);
  PCintPort::attachInterrupt(CENTER_BUTTON, control_buttons_interrupt, RISING);
  PCintPort::attachInterrupt(RIGHT_BUTTON, control_buttons_interrupt, RISING);
  PCintPort::attachInterrupt(LEFT_BUTTON, control_buttons_interrupt, RISING);
}


void loop() {
  if (game_state == GAME_INIT) {
    //the first frame of the game
    TFTscreen.stroke(blue_font, green_font, red_font);
    TFTscreen.text("Press green", 15, 50);
    TFTscreen.text("to play", 40, 80);
  } else if (game_state == GAME_BEGINS) {
    //clear the first frame of the game, the game begins
    TFTscreen.background(blue_background, green_background, red_background);
    TFTscreen.stroke(blue_font, green_font, red_font);
    
    //draw the table
    TFTscreen.rect(20, 30, RECT_WIDTH, RECT_HEIGHT);
    draw_lines();
    
    game_state = GAME_STARTED;
    
    //draw the circle selector, red color is the first player's color 0,0,255

    //set the interrupts
    init_interrupts_for_game_start();

    //print the player's turn text
    TFTscreen.setTextSize(1);
    TFTscreen.stroke(blue_font, green_font, red_font);
    TFTscreen.text("Player turn: ", 0, 0);

    //draw the circle selector, red color is the first player's color 0,0,255
    TFTscreen.stroke(0, 0, 255);
    TFTscreen.circle(SELECTOR_START_X , SELECTOR_START_Y, 5);
    // draw the number representing player's turn
    TFTscreen.text(player_turn, PLAYER_TURN_TEXT_X_POS, 0);
  }
  
  if (player_switched == 1) {
  //clear the player's turn with black
  clear_text_and_set_color(blue_font, green_font, red_font, player_turn, PLAYER_TURN_TEXT_X_POS, 0);
  
  //draw the player's disk
  //y is pos + 3 because center is 0 and are 7 columns
  
    for (int i = 0 ; i <= 5 ; i++) {
      set_color_by_player();
      // the element is occupied
      if (game_table[i][circle_position + 3] == 1) {
        // it is not the first line, so it's another unoccupied element above
         if (i != 0) {
          occupy_element(i - 1, circle_position + 3);
          // i is 0 so the entire column is full
         } else {
          // switch the player so this function does not loop, but the player stays the same
          player_switched = 0;
          TFTscreen.text(player_turn, PLAYER_TURN_TEXT_X_POS, 0);
         }
         break;
      } else {
        // element is 0, last line
        if (i == 5) {
          occupy_element(i, circle_position + 3);
          break;
        }
      }
    }
  }

  // move the circle left or right
  if(circle_state != STANDING) {
    move_circle();
    circle_state = STANDING;
  }
  
}


void control_buttons_interrupt() {
  if ((PIND & (1 << PD4)) != 0) {
    if (millis() - right_button_last_debounce_time >= 150) {
      right_button_last_debounce_time = millis();
      circle_state = 1;
    }
  } else if ((PIND & (1 << PD3)) != 0) {
    if (millis() - center_button_last_debounce_time >= 150) {
      center_button_last_debounce_time = millis();
      //switch player
      player_switched = 1;
    }
  } else if ((PIND & (1 << PD2)) != 0) {
    if (millis() - left_button_last_debounce_time >= 150) {
      left_button_last_debounce_time = millis();
      circle_state = -1;
    }
  }
}

void game_setup_interrupt() {
  if ((PIND & (1 << PD3)) != 0) {
    if (millis() - center_button_last_debounce_time >= 150) {
      center_button_last_debounce_time = millis();
      if (game_state == GAME_INIT) {
        game_state = GAME_BEGINS;
      }
    }
  }
}
