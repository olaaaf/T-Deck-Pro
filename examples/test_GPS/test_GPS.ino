

#include <TinyGPS++.h>

#define BOARD_GPS_EN 39 // enable GPS module
#define BOARD_GPS_RXD 44
#define BOARD_GPS_TXD 43
#define BOARD_GPS_PPS 1
#define SerialMon Serial
#define SerialGPS Serial2

// The TinyGPSPlus object
TinyGPSPlus gps;

void setup(void)
{
    // enable GPS module
    pinMode(BOARD_GPS_EN, OUTPUT);
    digitalWrite(BOARD_GPS_EN, HIGH);

    SerialMon.begin(38400);
    SerialGPS.begin(38400, SERIAL_8N1, BOARD_GPS_RXD, BOARD_GPS_TXD);

    delay(1500);
}

void loop(void)
{
    while (SerialGPS.available())
    {
        SerialMon.write(SerialGPS.read());
    }
    while (SerialMon.available())
    {
        SerialGPS.write(SerialMon.read());
    }
}