// Only certain pins can be used for RX (GPS/FONA TX)
// Leonardo: 8, 9, 10, 11, 14, 15, 16
#define FONA_RX 2
#define FONA_TX 9
#define FONA_RST 5
#define FONA_KEY 3
#define FONA_PS 4

#define GPS_TX 8
#define GPS_RX 5
#define GPS_PWR 7

#define LED 13
// Interrupts are tied to different pins
// Leonardo: int0=3, int1=2, int2=0, int3=1, int4=7 
// Pro mini: int0=2, int1=3
#define BUTTON 1
#define INTERRUPT 3  // Dependant on BUTTON
