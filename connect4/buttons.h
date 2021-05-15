
class Button {
  public:
    unsigned long last_debounce_time;
    char pin;
    char state;
    char last_state;

  void init_button(int pin);
  void toggle_button();
    
};
