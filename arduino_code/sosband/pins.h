// Only certain pins can be used for RX (GPS/FONA TX)
// Leonardo: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI)
// Pro Micro: 8, 9, 10, 14, 15, 16
#define FONA_RX 8
#define FONA_TX 9
#define FONA_KEY 5
#define FONA_PS 6
#define FONA_RST 7
#define FONA_VIO 4

#define GPS_TX 10
#define GPS_RX 16
#define GPS_PWR 14

#define LED 17
// Interrupts are tied to different pins
// Leonardo: int0=3, int1=2, int2=0, int3=1, int4=7
// Pro mini: int0=2, int1=3
// Pro micro: int0=3, int1=2, int2=RXI, int4=7

#define BUTTON 3
#define INTERRUPT 0  // Dependant on BUTTON

#define ADAFRUIT_FONA_DEBUG 1
