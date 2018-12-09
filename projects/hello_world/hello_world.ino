#include <timer.h>

#include <FastLED.h>
#include <buttons.h>

#define NUM_LEDS 12
#define NUM_COLORS 8
#define TIME 8
#define BRIGHT 9
#define COLOR 10

int mode = 0;

// time
int milliseconds = 0;
int seconds = 00;
int minutes = 0;
int hours = 11;
auto timer = timer_create_default();

// LED
CRGB leds[NUM_LEDS];
int pulse_milliseconds = 0;
int pulse_frequency = 0;
bool pulse_increasing = true;
Button push_buttons[3];
int brightness = 5;
int selected_color = 0;
CRGB available_colors[8] = {
  CRGB::Red,
  CRGB::Blue,
  CRGB::Green,
  CRGB::Aqua,
  CRGB::Coral,
  CRGB::Teal,
  CRGB::YellowGreen,
  CRGB::Moccasin
};
int blink_counter = 0;
int blink_frequency = 500;
bool led_status[2] = {false, false};

void tick() {
  milliseconds++;
  pulse_milliseconds++;
  pulse_milliseconds %= 255;
  String time_string;
  seconds += 1;
  minutes += seconds / 60;
  seconds = seconds % 60;
  hours += minutes / 60;
  minutes = minutes % 60;
  hours = hours % 12;
  time_string = (String)hours + (String)":" + (String)minutes + (String)":" + (String)seconds;
  Serial.println(time_string);
}

void pulse_led(int led_number){
  leds[led_number] = CRGB::Blue;
  leds[led_number].fadeToBlackBy( 192 );
  if (pulse_milliseconds % 30 == 0) {
    pulse_milliseconds = 0;
    if (pulse_frequency == 8) {
      pulse_increasing = false;
    }
    if (pulse_frequency == 0) {
      pulse_increasing = true;
    }
    if (pulse_increasing) {
      pulse_frequency++;
    } else {
      pulse_frequency--;
    }
  }
  for (int i = 0; i < pulse_frequency; i ++) {
    leds[led_number].nscale8( 192 );
  }
}

void change_brightness() {
  brightness += 50;
  if (brightness > 255) {
    brightness = 5;
  }
  FastLED.setBrightness(brightness);
}

int check_button_click(int button_pin) {
  int button_index = button_pin % 8;
  switch(push_buttons[button_index].check()) {
    case ON:
      return 1;
      break;
    case Hold:
      return 2;
      break;
    default:
      return 3;
      break;
  }
}

void switch_color() {
  selected_color++;
  selected_color %= NUM_COLORS;
}

void blink_led(uint8_t pin){
  if (!led_status[pin - 2]) {
    digitalWrite(pin, HIGH);
    led_status[pin - 2] = true;
  } else {
    digitalWrite(pin, LOW);
    led_status[pin - 2] = false;
  }
}

void turn_off_led(uint8_t pin) {
  led_status[pin - 2] = false;
  digitalWrite(pin, LOW);
}

void set_mode_0() {
  mode = 0;
  blink_counter = 0;
  turn_off_led(2);
  turn_off_led(3);
}

void setup() {
  // LEDs setup
  FastLED.addLeds<NEOPIXEL, 13>(leds, NUM_LEDS);

  // timer setup
  timer.every(1000, tick);

  // buttons setup
  push_buttons[0].assign(TIME);
  push_buttons[1].assign(BRIGHT);
  push_buttons[2].assign(COLOR);
  for (int i = 0; i < 3; i++) {
    push_buttons[i].setMode(OneShotTimer);
    push_buttons[i].setTimer(1500);
    push_buttons[i].setRefresh(500);
  }
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  FastLED.setBrightness(brightness);
  Serial.begin(9600);
}

void loop() {
  timer.tick();
  
  // LEDs
  FastLED.clear();
  
//  if(mode == 0) {
//    int color_change = check_button_click(COLOR);
//    if(color_change == 1) {
//      switch_color();
//    }
//    int brightness_change = check_button_click(BRIGHT);
//    if(brightness_change == 1) {
//      change_brightness();
//    }
//  }
  int color_change = 0;
  int brightness_change = 0;
  switch(mode) {
    
    case 0:
      color_change = check_button_click(COLOR);
      if(color_change == 1) {
        switch_color();
      }
      brightness_change = check_button_click(BRIGHT);
      if(brightness_change == 1) {
        change_brightness();
      }
      break;
    case 1:
      color_change = check_button_click(COLOR);
      if(color_change == 1) {
        switch_color();
      }
      brightness_change = check_button_click(BRIGHT);
      if(brightness_change == 1) {
        change_brightness();
      }
      break;
    default:
      break;
  }
  for (int i = 0; i <= hours; i++) {
    leds[i] = available_colors[selected_color];
  }
  if(minutes == 0 && seconds < 10) {
    pulse_led(hours); 
  }
  
  if(blink_counter % blink_frequency == 0 && mode >= 1) {
    blink_led(2);
  }
  if(blink_counter % blink_frequency == 0 && mode >= 2) {
    blink_led(3);
  }
  blink_counter++;
  digitalWrite(4, HIGH);

  int time_set = check_button_click(TIME);
  if(time_set == 2) {
    mode = 1;
  }
  if(mode == 1 && time_set == 1) {
    set_mode_0();
  }
  
  FastLED.show();
}
