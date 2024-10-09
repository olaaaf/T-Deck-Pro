#include "Arduino.h"
#include "Audio.h"
#include "SD.h"
#include "SPI.h"
#include "FS.h"
#include "Ticker.h"
#include <stdio.h>
#include "utilities.h"

// Digital I/O used
#define SD_CS 48
#define SPI_MOSI 33
#define SPI_MISO 47
#define SPI_SCK 36
#define I2S_BCLK 7
#define I2S_DOUT 8
#define I2S_LRC 9

#define ASSERT_FLAG(x) \
    if (x == false)    \
        Serial.printf("[%d] Execution error\n", __LINE__);

Audio audio;
Ticker ticker;
struct tm timeinfo;
time_t now;

uint8_t hour = 6;
uint8_t minute = 59;
uint8_t sec = 45;

bool f_time = false;
int8_t timefile = -1;
char chbuf[100];
bool audio_paly_flag = false;

void tckr1s()
{
    sec++;
    if (sec > 59)
    {
        sec = 0;
        minute++;
    }
    if (minute > 59)
    {
        minute = 0;
        hour++;
    }
    if (hour > 23)
    {
        hour = 0;
    }
    if (minute == 59 && sec == 50)
        f_time = true; // flag will be set 10s before full hour
    Serial.printf("%02d:%02d:%02d\n", hour, minute, sec);
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(fs, file.path(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void sd_card_init(void)
{
    if (!SD.begin(SD_CS))
    {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else
    {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/voice_time", 0);
}

void setup()
{
    Serial.begin(115200);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    sd_card_init();

    audio_paly_flag = audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    ASSERT_FLAG(audio_paly_flag);

    audio.setVolume(21); // 0...21
    ticker.attach(1, tckr1s);

    pinMode(BOARD_6609_EN, OUTPUT);
    digitalWrite(BOARD_6609_EN, HIGH);
}

void loop()
{
    audio.loop();
    if (f_time == true)
    {
        f_time = false;
        timefile = 3;
        uint8_t next_hour = hour + 1;
        if (next_hour == 25)
            next_hour = 1;
        Serial.printf("Paly Audio\n");
        audio_paly_flag = audio.connecttoFS(SD, "/voice_time/BBIBBI.mp3");
        ASSERT_FLAG(audio_paly_flag);
    }
}

void audio_eof_mp3(const char *info)
{ // end of file
    // Serial.printf("file :%s\n", info);
    if (timefile > 0)
    {
        Serial.printf("timefile: %d\n", timefile);
        if (timefile == 1)
        {
            audio_paly_flag = audio.connecttoFS(SD, "/voice_time/2-on.mp3");
            ASSERT_FLAG(audio_paly_flag);
            timefile--;
        } // stroke
        if (timefile == 2)
        {
            audio_paly_flag = audio.connecttoFS(SD, "/voice_time/2-on.mp3");
            ASSERT_FLAG(audio_paly_flag);
            timefile--;
        } // precisely
        if (timefile == 3)
        {
            audio_paly_flag = audio.connecttoFS(SD, "/voice_time/2-on.mp3");
            ASSERT_FLAG(audio_paly_flag);
            timefile--;
        }
    }
}

// optional
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}
