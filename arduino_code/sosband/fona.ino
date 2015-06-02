#define NUMBERS_URL "46.101.35.75/api/arduino/numbers\0"
#define GPS_DATA_URL "46.101.35.75/api/arduino/gps\0"
#define APN "internet"
#define APN_USERNAME "guest"
#define APN_PASSWORD ""

#include "pins.h"
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

void fona_setup() {
  /* Performs initial setup for the FONA */
  pinMode(FONA_PS, INPUT);
  pinMode(FONA_KEY, OUTPUT);
  digitalWrite(FONA_KEY, HIGH);
  while(digitalRead(FONA_PS) == LOW) {
    digitalWrite(FONA_KEY, LOW);
    delay(2500);
    digitalWrite(FONA_KEY, HIGH);
    delay(500);
  }
  fonaSS.begin(4800);
  fona.begin(fonaSS);
  
  fona.setGPRSNetworkSettings(F(APN), F(APN_USERNAME), F(APN_PASSWORD));
  
  while(!fona.enableGPRS(true)) {
    fona.enableGPRS(false);
  }
  
  while(!fona.setVolume(100));
  while(!fona.setAudio(FONA_EXTAUDIO));
  while(!fona.setMicVolume(FONA_EXTAUDIO, 10));
}

void fona_shutdown() {
  /* Puts the FONA in low power mode for shutting down */
  if(digitalRead(FONA_PS) == HIGH) {
    digitalWrite(FONA_KEY, LOW);
    delay(2500);
    digitalWrite(FONA_KEY, HIGH);
  }
  fonaSS.flush();
  fonaSS.end();
}

void get_e_numbers(char number1[], char number2[]) {
  /* Gets emergency numbers from the HTTP API
     Required: number1 and number2 to be large enough
     for the number */

  uint16_t statuscode, len, i = 0;

  fona.HTTP_GET_start(NUMBERS_URL, &statuscode, &len);

  // Get first number
  char c;
  do {
    if(!fona.available()){continue;}
    c = fona.read();
    number1[i++] = c;
    len--;
  } while (c != '\n' && len > 0);
  number1[i - 1] = '\0';
  
  // Get second number
  i = 0;
  while (len > 0) {
    if(!fona.available()){continue;}
    c = fona.read();
    number2[i++] = c;
    len--;
  }
  number2[i] = '\0';
  fona.HTTP_GET_end();
  Serial.println("Got numbers");
}

void post_GPS_data(char data[]) {
  /* POSTS a string representing GPS data to the HTTP API
     String should be ASCII null terminated */
  uint16_t statuscode, len;
  
  if(fona.HTTP_POST_start(GPS_DATA_URL, F("text/plain"), (uint8_t *) data, strlen(data), &statuscode, &len)) {
    Serial.print("Send successful");
  } else {
    Serial.println("Send unsuccessful");
    return;
  }
  while (len > 0) {
    while(fona.available()) {
      char c = fona.read();
      len--;
      if(!len) break;
    }
  }
  fona.HTTP_POST_end();
}

void start_call(char number[]) {
  
  fona.callPhone(number);
}

void end_call() {
  fona.hangUp();
}
