#include "pins.h"

uint8_t e_number1[30]; // Primary emergency number
uint8_t e_number2[30]; // Secondary emergency number

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  fona_setup();
  get_e_numbers(e_number1, e_number2);
  gps_setup();
  digitalWrite(LED, LOW);
  // call test
  //start_call(e_number1);
  //delay(10000);
  //end_call();
}

void loop() {
  uint8_t * gps_data = read_gps_char();
  if(gps_data && is_valid_gps_command(gps_data)) {
    post_GPS_data(gps_data);
  }
}
