#include "test_factory.h"
#include <TinyGPS++.h>


#define SerialAT Serial1
#define SerialGPS Serial2

//----------------------------------------------------------------------------
#if 1 // A7682E
#define TINY_GSM_MODEM_SIM7672
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define MODEM_GPS_ENABLE_GPIO               (-1)

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);

bool A7682E_init(void)
{
    Serial.println("Place your board outside to catch satelite signal");

    // Set module baud rate and UART pins
    SerialAT.begin(115200, SERIAL_8N1, BOARD_A7682E_TXD, BOARD_A7682E_RXD);

    Serial.println("Start modem...");
    // delay(3000);

    int retry = 0;
    while (!modem.testAT(1000)) {
        Serial.println(".");
        if (retry++ > 10) {
            digitalWrite(BOARD_A7682E_PWRKEY, LOW);
            delay(100);
            digitalWrite(BOARD_A7682E_PWRKEY, HIGH);
            delay(1000);
            digitalWrite(BOARD_A7682E_PWRKEY, LOW);
            retry = 0;
        }
    }
    Serial.println();
    delay(200);

    Serial.println("Enabling GPS/GNSS/GLONASS");
    // while (!modem.enableGPS(MODEM_GPS_ENABLE_GPIO)) {
    //     Serial.print(".");
    // }
    Serial.println();
    Serial.println("GPS Enabled");

    return true;
}

void A7682E_loop(void)
{
    while (SerialAT.available()) {
        Serial.write(SerialAT.read());
    }
    while (Serial.available()) {
        SerialAT.write(Serial.read());
    }

    // float lat2      = 0;
    // float lon2      = 0;
    // float speed2    = 0;
    // float alt2      = 0;
    // int   vsat2     = 0;
    // int   usat2     = 0;
    // float accuracy2 = 0;
    // int   year2     = 0;
    // int   month2    = 0;
    // int   day2      = 0;
    // int   hour2     = 0;
    // int   min2      = 0;
    // int   sec2      = 0;
    // uint8_t    fixMode   = 0;
    // for (int8_t i = 15; i; i--) {
    //     Serial.println("Requesting current GPS/GNSS/GLONASS location");
    //     if (modem.getGPS(&fixMode, &lat2, &lon2, &speed2, &alt2, &vsat2, &usat2, &accuracy2,
    //                      &year2, &month2, &day2, &hour2, &min2, &sec2)) {

    //         Serial.print("FixMode:"); Serial.println(fixMode);
    //         Serial.print("Latitude:"); Serial.print(lat2, 6); Serial.print("\tLongitude:"); Serial.println(lon2, 6);
    //         Serial.print("Speed:"); Serial.print(speed2); Serial.print("\tAltitude:"); Serial.println(alt2);
    //         Serial.print("Visible Satellites:"); Serial.print(vsat2); Serial.print("\tUsed Satellites:"); Serial.println(usat2);
    //         Serial.print("Accuracy:"); Serial.println(accuracy2);

    //         Serial.print("Year:"); Serial.print(year2);
    //         Serial.print("\tMonth:"); Serial.print(month2);
    //         Serial.print("\tDay:"); Serial.println(day2);

    //         Serial.print("Hour:"); Serial.print(hour2);
    //         Serial.print("\tMinute:"); Serial.print(min2);
    //         Serial.print("\tSecond:"); Serial.println(sec2);
    //         break;
    //     } else {
    //         Serial.println("Couldn't get GPS/GNSS/GLONASS location, retrying in 15s.");
    //         delay(15000L);
    //     }
    // }
    // Serial.println("Retrieving GPS/GNSS/GLONASS location again as a string");
    // String gps_raw = modem.getGPSraw();
    // Serial.print("GPS/GNSS Based Location String:");
    // Serial.println(gps_raw);
    // Serial.println("Disabling GPS");

    // modem.disableGPS();
}
#endif 
//----------------------------------------------------------------------------
#if 1 // GPS
static void smartDelay(unsigned long ms);
static void printFloat(float val, bool valid, int len, int prec);
static void printInt(unsigned long val, bool valid, int len);
static void printDateTime(TinyGPSDate &d, TinyGPSTime &t);
static void printStr(const char *str, int len);

// The TinyGPS++ object
TinyGPSPlus gps;

bool GPS_init(void)
{
    SerialGPS.begin(38400, SERIAL_8N1, BOARD_GPS_RXD, BOARD_GPS_TXD);
    // SerialAT.begin(115200, SERIAL_8N1, BOARD_A7682E_TXD, BOARD_A7682E_RXD);

    delay(1500);

    Serial.println(F("FullExample.ino"));
    Serial.println(F("An extensive example of many interesting TinyGPS++ features"));
    Serial.print(F("Testing TinyGPS++ library v. "));
    Serial.println(TinyGPSPlus::libraryVersion());
    Serial.println(F("by Mikal Hart"));
    Serial.println();
    Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
    Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
    Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));

    return true;
}

void GPS_get(float *lat, float *lon, float *speed, float *alt, float *accuracy,
             int *vsat,  int *usat,  int *year,    int *month, int *day,
             int *hour,  int *min,   int *sec)
{
    *lat      = gps.satellites.isValid() ? gps.location.lat() : 0;
    *lon      = gps.location.isValid() ? gps.location.lng() : 0;
    *speed    = gps.speed.isValid() ? gps.speed.kmph() : 0;
    *alt      = gps.altitude.isValid() ? gps.altitude.meters() : 0;
    *accuracy = gps.course.isValid() ? gps.course.deg() : 0;
    *vsat     = gps.altitude.isValid() ? gps.satellites.value() : 0;
    *usat     = gps.altitude.isValid() ? gps.satellites.value() : 0;
    *year     = gps.date.isValid() ? gps.date.year() : 0;
    *month    = gps.date.isValid() ? gps.date.month() : 0;
    *day      = gps.date.isValid() ? gps.date.day() : 0;
    *hour     = gps.time.isValid() ? gps.time.hour() : 0;
    *min      = gps.time.isValid() ? gps.time.minute() : 0;
    *sec      = gps.time.isValid() ? gps.time.second() : 0;
}

void GPS_loop(void)
{
    // while (1)
    // {
        while (SerialGPS.available())
        {
            Serial.write(SerialGPS.read());
        }
        while (Serial.available())
        {
            SerialGPS.write(Serial.read());
        }
    // }

    // static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

    // printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
    // printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
    // printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
    // printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
    // printInt(gps.location.age(), gps.location.isValid(), 5);
    // printDateTime(gps.date, gps.time);
    // printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
    // printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
    // printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
    // printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

    // unsigned long distanceKmToLondon =
    //     (unsigned long)TinyGPSPlus::distanceBetween(
    //         gps.location.lat(),
    //         gps.location.lng(),
    //         LONDON_LAT,
    //         LONDON_LON) /
    //     1000;
    // printInt(distanceKmToLondon, gps.location.isValid(), 9);

    // double courseToLondon =
    //     TinyGPSPlus::courseTo(
    //         gps.location.lat(),
    //         gps.location.lng(),
    //         LONDON_LAT,
    //         LONDON_LON);

    // printFloat(courseToLondon, gps.location.isValid(), 7, 2);

    // const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);

    // printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

    // printInt(gps.charsProcessed(), true, 6);
    // printInt(gps.sentencesWithFix(), true, 10);
    // printInt(gps.failedChecksum(), true, 9);
    // Serial.println();

    // smartDelay(1000);

    // if (millis() > 5000 && gps.charsProcessed() < 10)
    //     Serial.println(F("No GPS data received: check wiring"));
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        while (SerialGPS.available())
            gps.encode(SerialGPS.read());
    } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
    if (!valid)
    {
        while (len-- > 1)
            Serial.print('*');
        Serial.print(' ');
    }
    else
    {
        Serial.print(val, prec);
        int vi = abs((int)val);
        int flen = prec + (val < 0.0 ? 2 : 1); // . and -
        flen += vi >= 1000 ? 4 : vi >= 100 ? 3
                             : vi >= 10    ? 2
                                           : 1;
        for (int i = flen; i < len; ++i)
            Serial.print(' ');
    }
    smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
    char sz[32] = "*****************";
    if (valid)
        sprintf(sz, "%ld", val);
    sz[len] = 0;
    for (int i = strlen(sz); i < len; ++i)
        sz[i] = ' ';
    if (len > 0)
        sz[len - 1] = ' ';
    Serial.print(sz);
    smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
    if (!d.isValid())
    {
        Serial.print(F("********** "));
    }
    else
    {
        char sz[32];
        sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
        Serial.print(sz);
    }

    if (!t.isValid())
    {
        Serial.print(F("******** "));
    }
    else
    {
        char sz[32];
        sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
        Serial.print(sz);
    }

    printInt(d.age(), d.isValid(), 5);
    smartDelay(0);
}

static void printStr(const char *str, int len)
{
    int slen = strlen(str);
    for (int i = 0; i < len; ++i)
        Serial.print(i < slen ? str[i] : ' ');
    smartDelay(0);
}
#endif