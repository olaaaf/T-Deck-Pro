#include "Arduino.h"

// A7682E Modem
#define BOARD_A7682E_POWER_EN 41 // enable 7682 module
#define BOARD_A7682E_RI 7
#define BOARD_A7682E_DTR 8
#define BOARD_A7682E_RST 9
#define BOARD_A7682E_RXD 10
#define BOARD_A7682E_TXD 11
#define BOARD_A7682E_PWRKEY 40

#define SerialMon Serial
#define SerialAT Serial1

bool reply = false;

/**
 * After first time power on, some register of this pin will be written then
 * it will lose shutdown function, so it is recommended to use PWRKEY to power
 * on the module and RESET key only used as reset function.
 */
void A7682E_reset(void)
{
    digitalWrite(BOARD_A7682E_RST, LOW);
    delay(2500); // Pull down for at least 2 seconds to reset
    digitalWrite(BOARD_A7682E_RST, HIGH);
}
/**
 *
 */
void A7682E_power_on(void)
{
    digitalWrite(BOARD_A7682E_PWRKEY, LOW);
    delay(10);
    digitalWrite(BOARD_A7682E_PWRKEY, HIGH);
    delay(50);
    digitalWrite(BOARD_A7682E_PWRKEY, LOW);
    delay(10);
}
/**
 *
 */
void A7682E_power_off(void)
{
    digitalWrite(BOARD_A7682E_PWRKEY, LOW);
    delay(10);
    digitalWrite(BOARD_A7682E_PWRKEY, HIGH);
    delay(3000);
    digitalWrite(BOARD_A7682E_PWRKEY, LOW);
    delay(10);
}

void setup(void)
{
    // enable GPS module power
    pinMode(BOARD_A7682E_POWER_EN, OUTPUT);
    digitalWrite(BOARD_A7682E_POWER_EN, HIGH);

    SerialMon.begin(115200);
    SerialAT.begin(115200, SERIAL_8N1, BOARD_A7682E_TXD, BOARD_A7682E_RXD);

    //
    pinMode(BOARD_A7682E_PWRKEY, OUTPUT);
    digitalWrite(BOARD_A7682E_PWRKEY, HIGH);
    // pinMode(BOARD_A7682E_RST, OUTPUT);
    // digitalWrite(BOARD_A7682E_RST, HIGH);

    // A7682 Power off
    A7682E_power_off();

    // A7682  Power on
    A7682E_power_on();

    int i = 10;
    Serial.println("\nTesting Modem Response...\n");
    Serial.println("****");
    while (i)
    {
        SerialAT.println("AT");
        delay(500);
        if (SerialAT.available())
        {
            String r = SerialAT.readString();
            SerialAT.println(r);
            if (r.indexOf("OK") >= 0)
            {
                reply = true;
                break;
            }
        }
        delay(500);
        i--;
    }
    Serial.println("****\n");

    if (reply)
    {
        Serial.println(F("***********************************************************"));
        Serial.println(F(" You can now send AT commands"));
        Serial.println(F(" Enter \"AT\" (without quotes), and you should see \"OK\""));
        Serial.println(F(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
        Serial.println(F(" DISCLAIMER: Entering AT commands without knowing what they do"));
        Serial.println(F(" can have undesired consiquinces..."));
        Serial.println(F("***********************************************************\n"));
    }
    else
    {
        Serial.println(F("***********************************************************"));
        Serial.println(F(" Failed to connect to the modem! Check the baud and try again."));
        Serial.println(F("***********************************************************\n"));
    }
}

void loop(void)
{
    while (SerialAT.available())
    {
        SerialMon.write(SerialAT.read());
    }
    while (SerialMon.available())
    {
        SerialAT.write(SerialMon.read());
    }
}