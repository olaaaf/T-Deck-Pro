<h1 align = "center">🏆T-Deck-Pro🏆</h1>

<p> 
<!-- <img src="https://img.shields.io/badge/ESP—IDF-5.1.1-ff3034" height="20px"></a> -->
<img src="https://img.shields.io/badge/PlatformIO-6.5.0-ff7f00" height="20px"></a>
<img src="https://img.shields.io/badge/Arduino-2.0.14-008284" height="20px"></a>
</p>

![Build Status](https://github.com/Xinyuan-LilyGO/T-Deck-Pro/actions/workflows/platformio.yml/badge.svg?event=push)

Under development...
Please ignore this warehouse!


## :zero: Version 🎁

### 1、Version

### 2、Where to buy.

## :one: Product 🎁

|       H693       |           T-Deck-Pro           |
| :--------------: | :----------------------------: |
|       MCU        |            ESP32-S3            |
|  Flash / PSRAM   |            16M / 8M            |
|       LoRa       |             SX1262             |
|       GPS        |            MIA-M10Q            |
|     Display      |      GDEQ031T10 (320x240)      |
|    4G-Module     |       A7682E  🟢Optional       |
|      Audio       |       PCM512A 🟢Optional       |
| Battery Capacity |          3.7V-1500mAh          |
|   Battery Chip   | BQ25896 (0x6B), BQ27220 (0x55) |
|      Touch       |         CST328 (0x1A)          |
|    Gyroscope     |        BHI260AP (0x28)         |
|     Keyboard     |         TCA8418 (0x34)         |
|   Light sensor   |       LTR_553ALS (0x23)        |

A7682E https://en.simcom.com/product/A7682E.html

## :two: Module 🎁

Datasheets on the chip are available in [./hardware](./hardware/) directory.

## :three: Quick Start 🎁

🟢 PlatformIO is recommended because these examples were developed on it. 🟢 

### 1、PlatformIO

1. Install [Visual Studio Code](https://code.visualstudio.com/) and [Python](https://www.python.org/), and clone or download the project;
2. Search for the `PlatformIO` plugin in the `VisualStudioCode` extension and install it;
3. After the installation is complete, you need to restart `VisualStudioCode`
4. After opening this project, PlatformIO will automatically download the required tripartite libraries and dependencies, the first time this process is relatively long, please wait patiently;
5. After all the dependencies are installed, you can open the `platformio.ini` configuration file, uncomment in `example` to select a routine, and then press `ctrl+s` to save the `.ini` configuration file;
6. Click :ballot_box_with_check: under VScode to compile the project, then plug in USB and select COM under VScode;
7. Finally, click the :arrow_right:  button to download the program to Flash;

### 2、Arduino IDE

1. Install [Arduino IDE](https://www.arduino.cc/en/software)

2. Copy all files under `this project/lib/` and paste them into the Arduion library path (generally `C:\Users\YourName\Documents\Arduino\libraries`);

3. Open the Arduion IDE and click `File->Open` in the upper left corner to open an example in `this project/example/xxx/xxx.ino` under this item;

4. Then configure Arduion. After the configuration is completed in the following way, you can click the button in the upper left corner of Arduion to compile and download;

| Arduino IDE Setting                  | Value                              |
| ------------------------------------ | ---------------------------------- |
| Board                                | ***ESP32S3 Dev Module***           |
| Port                                 | Your port                          |
| USB CDC On Boot                      | Enable                             |
| CPU Frequency                        | 240MHZ(WiFi)                       |
| Core Debug Level                     | None                               |
| USB DFU On Boot                      | Disable                            |
| Erase All Flash Before Sketch Upload | Disable                            |
| Events Run On                        | Core1                              |
| Flash Mode                           | QIO 80MHZ                          |
| Flash Size                           | **16MB(128Mb)**                    |
| Arduino Runs On                      | Core1                              |
| USB Firmware MSC On Boot             | Disable                            |
| Partition Scheme                     | **16M Flash(3M APP/9.9MB FATFS)**  |
| PSRAM                                | **OPI PSRAM**                      |
| Upload Mode                          | **UART0/Hardware CDC**             |
| Upload Speed                         | 921600                             |
| USB Mode                             | **CDC and JTAG**                   |


## :four: Pins 🎁

~~~c
// Serial
#define SerialMon   Serial      // 
#define SerialAT    Serial1     // 
#define SerialGPS   Serial2     // 

// IIC Addr
#define BOARD_I2C_ADDR_TOUCH      0x1A // Touch        --- CST328
#define BOARD_I2C_ADDR_LTR_553ALS 0x23 // Light sensor --- LTR_553ALS
#define BOARD_I2C_ADDR_GYROSCOPDE 0x28 // Gyroscope    --- BHI260AP
#define BOARD_I2C_ADDR_KEYBOARD   0x34 // Keyboard     --- TCA8418
#define BOARD_I2C_ADDR_BQ27220    0x55 //
#define BOARD_I2C_ADDR_BQ25896    0x6B //

// IIC
#define BOARD_I2C_SDA  13
#define BOARD_I2C_SCL  14

#define BOARD_I2S_BCLK 7
#define BOARD_I2S_DOUT 8
#define BOARD_I2S_LRC 9

// Keyboard
#define BOARD_KEYBOARD_SCL BOARD_I2C_SCL
#define BOARD_KEYBOARD_SDA BOARD_I2C_SDA
#define BOARD_KEYBOARD_INT 15
#define BOARD_KEYBOARD_LED 42

// Touch
#define BOARD_TOUCH_SCL BOARD_I2C_SCL
#define BOARD_TOUCH_SDA BOARD_I2C_SDA
#define BOARD_TOUCH_INT 12
#define BOARD_TOUCH_RST 45

// LTR-553ALS-WA  beam sensor
#define BOARD_ALS_SCL BOARD_I2C_SCL
#define BOARD_ALS_SDA BOARD_I2C_SDA
#define BOARD_ALS_INT 16

// Gyroscope
#define BOARD_GYROSCOPDE_SCL BOARD_I2C_SCL
#define BOARD_GYROSCOPDE_SDA BOARD_I2C_SDA
#define BOARD_GYROSCOPDE_INT 21
#define BOARD_GYROSCOPDE_RST -1

// SPI
#define BOARD_SPI_SCK  36
#define BOARD_SPI_MOSI 33
#define BOARD_SPI_MISO 47

// Display
#define LCD_HOR_SIZE 240
#define LCD_VER_SIZE 320
#define DISP_BUF_SIZE (LCD_HOR_SIZE * LCD_VER_SIZE)

#define BOARD_EPD_SCK  BOARD_SPI_SCK
#define BOARD_EPD_MOSI BOARD_SPI_MOSI
#define BOARD_EPD_DC   35
#define BOARD_EPD_CS   34
#define BOARD_EPD_BUSY 37
#define BOARD_EPD_RST  -1

// SD card
#define BOARD_SD_CS   48
#define BOARD_SD_SCK  BOARD_SPI_SCK
#define BOARD_SD_MOSI BOARD_SPI_MOSI
#define BOARD_SD_MISO BOARD_SPI_MISO

// Lora
#define BOARD_LORA_SCK  BOARD_SPI_SCK
#define BOARD_LORA_MOSI BOARD_SPI_MOSI
#define BOARD_LORA_MISO BOARD_SPI_MISO
#define BOARD_LORA_CS   3
#define BOARD_LORA_BUSY 6
#define BOARD_LORA_RST  4
#define BOARD_LORA_INT  5

// GPS
#define BOARD_GPS_RXD 44
#define BOARD_GPS_TXD 43
#define BOARD_GPS_PPS 1

// A7682E Modem
#define BOARD_A7682E_RI     7
#define BOARD_A7682E_ITR    8
#define BOARD_A7682E_RST    9
#define BOARD_A7682E_RXD    10
#define BOARD_A7682E_TXD    11
#define BOARD_A7682E_PWRKEY 40

// Boot pin
#define BOARD_BOOT_PIN  0

// Motor pin
#define BOARD_MOTOR_PIN 2

// EN
#define BOARD_GPS_EN  39  // enable GPS module
#define BOARD_1V8_EN  38  // enable gyroscope module
#define BOARD_6609_EN 41  // enable 7682 module
#define BOARD_LORA_EN 46  // enable LORA module

// Mic
#define BOARD_MIC_DATA        17
#define BOARD_MIC_CLOCK       18
// -------------------------------------------------
~~~



## :five: Test 🎁


## :six: FAQ 🎁


## :seven: Schematic & 3D 🎁

For more information, see the `./hardware` directory.

