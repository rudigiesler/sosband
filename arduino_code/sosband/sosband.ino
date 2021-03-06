#define DEBOUNCE_TIME 50
#define NUMBER2_TIME 3000
#define TURNOFF_TIME 10000

#include "pins.h"
#include <avr/sleep.h>

char e_number1[20]; // Primary emergency number
char e_number2[20]; // Secondary emergency number

uint16_t button_down_time = 0, button_up_time = 0;
uint8_t last_button_state = HIGH;
uint8_t call_in_progress = false;

char reading;
char power_off = false;
uint16_t duration;


void wakeup() {
  // Wake up
}

void button_change() {
  reading = digitalRead(BUTTON);
  
  // Test for button pressed
  duration = millis() - button_up_time;
  if(reading == LOW && last_button_state == HIGH && duration > DEBOUNCE_TIME) {
    button_down_time = millis();
  }
  
  duration = millis() - button_down_time;
  // Test for button released
  if(reading == HIGH && last_button_state == LOW && duration > DEBOUNCE_TIME) {
    button_up_time = millis();
  }
}
  

void switch_to_gps() {
    pause_fona_serial();
    gps_resume_serial();
}

void switch_to_fona() {
  gps_pause_serial();
  resume_fona_serial();
}


void setup() {
  Serial.begin(115200);
  Serial.println(F("Serial Connected"));
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
  attachInterrupt(INTERRUPT, button_change, CHANGE);
  Serial.println("Setup complete");
}

void turn_off() {
  digitalWrite(LED, LOW);
  delay(500);
  digitalWrite(LED, HIGH);
  fona_shutdown();
  gps_shutdown();
  digitalWrite(LED, LOW);
  
  sleep_enable();
  attachInterrupt(INTERRUPT, wakeup, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  Serial.flush();
  Serial.end();
  sleep_cpu();
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
  char * gps_data = read_gps_char();
  if(!call_in_progress && gps_data && is_valid_gps_command(gps_data)) {
    digitalWrite(LED, LOW);
    post_GPS_data(gps_data);
    digitalWrite(LED, HIGH);
  }
  
  if(reading == HIGH && last_button_state == LOW && duration > DEBOUNCE_TIME) {
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
  
  duration = millis() - button_down_time;
    // Test for power off
  if(reading == LOW && duration > TURNOFF_TIME) {
    button_down_time = millis();
    last_button_state = HIGH;
    reading = HIGH;
    turn_off();
  }

  last_button_state = reading;
 
}
