#define DEBOUNCE_TIME 50
#define NUMBER2_TIME 3000
#define TURNOFF_TIME 10000

#include "pins.h"
#include <avr/sleep.h>

uint8_t e_number1[30]; // Primary emergency number
uint8_t e_number2[30]; // Secondary emergency number

uint16_t button_down_time = 0, button_up_time = 0;
uint8_t last_button_state = HIGH;
uint8_t call_in_progress = false;


void wakeup() {
  // Wake up
}


void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);
  digitalWrite(LED, HIGH);
  fona_setup();
  get_e_numbers(e_number1, e_number2);
  gps_setup();
  digitalWrite(LED, LOW);
  delay(500);
  digitalWrite(LED, HIGH);
  // call test
  //start_call(e_number1);
  //delay(10000);
  //end_call();
}

void turn_off() {
  digitalWrite(LED, LOW);
  delay(500);
  digitalWrite(LED, HIGH);
  fona_shutdown();
  gps_shutdown();
  digitalWrite(LED, LOW);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(INTERRUPT, wakeup, LOW);
  sleep_mode();
  
  //return from interrupt
  sleep_disable();
  detachInterrupt(INTERRUPT);
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  setup();
  digitalWrite(LED, HIGH);
  
}

void loop() {
  // Get and send GPS data
  uint8_t * gps_data = read_gps_char();
  if(gps_data && is_valid_gps_command(gps_data)) {
    post_GPS_data(gps_data);
  }
  
  // Check button
  uint8_t reading = digitalRead(BUTTON);
  uint16_t duration = millis() - button_up_time;
  // Test for button pressed
  if(reading == LOW && last_button_state == HIGH && duration > DEBOUNCE_TIME) {
   button_down_time = millis();
  }
  // Test for button release
  duration = millis() - button_down_time;
  if(reading == HIGH && last_button_state == LOW && duration > DEBOUNCE_TIME) {
    button_up_time = millis();
    duration = button_up_time - button_down_time;
    if (call_in_progress) {
      call_in_progress = false;
      end_call();
    } else if (duration < NUMBER2_TIME) {
      // Call number 1
      call_in_progress = true;
      start_call(e_number1);
    } else {
      // Call number 2
      call_in_progress = true;
      start_call(e_number2);
    }
  }
  // Test for power off
  if(reading == LOW && duration > TURNOFF_TIME) {
    button_down_time = millis();
    turn_off();
  }
  
  last_button_state = reading;
 
}
