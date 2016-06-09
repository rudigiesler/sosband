#include "pins.h"
#include <SoftwareSerial.h>

SoftwareSerial gpsSS = SoftwareSerial(GPS_TX, GPS_RX);
char textbuffer[255]; // large buffer for sending/receiving text
uint16_t textbufferpos = 0;

void gps_setup() {
  /* Performs initial setup for the GPS unit */
  pinMode(GPS_PWR, OUTPUT);
  digitalWrite(GPS_PWR, HIGH);
  gpsSS.begin(9600);
}

void gps_shutdown() {
  /* Puts the GPS into low power mode for shutting down */
  digitalWrite(GPS_PWR, LOW);
  gpsSS.flush();
  gpsSS.end();
}

void gps_pause_serial() {
  gpsSS.flush();
}

void gps_resume_serial() {
  gpsSS.listen();
}

char * read_gps_char() {
  /* Reads a single char from the serial port (if available) and places
     it on the buffer. If it is the end of a command, returns the address
     of the buffer. Else, returns 0. */
  if(gpsSS.available()) {
    uint8_t c = gpsSS.read();
    textbuffer[textbufferpos++] = c;
    if(c == '\n') {
      textbuffer[textbufferpos] = '\0';
      textbufferpos = 0;
      return textbuffer;
    }
  }
  return 0;
}

char is_gps_verb(char command[], const char verb[]) {
  // Checks to see if `command` is the GPS verb `verb`
  for(char i = 0; i < 6; i++) {
    if(command[i] != verb[i]) {
      return false;
    }
  }
  return true;
}

char is_valid_gprmc(char command[]) {
  // Checks that the GPS data valid flag is correct
  char pos = 0;
  while (command[pos++] != ',') {
   if(pos > 10) {
    return false;
   }
  } // Message ID
  while (command[pos++] != ',') {
   if (pos > 20) {
    return false;
   }
  } // UTC time
  return command[pos] == 'A';
}

char is_valid_gps_command(char command[]) {
  // This doesn't actually check for all valid GPS commands,
  // it just checks that it's the one command we're interested in,
  // and that the data is valid.
  return is_gps_verb(command, "$GPRMC") && is_valid_gprmc(command);
}
