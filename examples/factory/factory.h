#pragma once
/*********************************************************************************
 *                              include
 *********************************************************************************/
#include "utilities.h"
#include <esp32-hal-log.h>
#include <GxEPD2_BW.h>
#include <Adafruit_TCA8418.h>
#include <XPowersLib.h>

// SD card
#include "FS.h"
#include "SD.h"
#include "SPI.h"

/*********************************************************************************
 *                              macro
 *********************************************************************************/
#define CLOCK_24_HOUR_FORMAT 1
#define CLOCK_12_HOUR_FORMAT 2
#define CLOCK_HOUR_FORMAT CLOCK_24_HOUR_FORMAT

#define KEYPAD_PRESS   1
#define KEYPAD_RELEASE 0

// EDP
#define EDP_TEST_MAX_ITEM  14
#define DISPLAY_ALIGN_CENTER 0
#define DISPLAY_ALIGN_LEFT   1
#define DISPLAY_ALIGN_RIGHT  2

#define LORA_MODE_SEND 0
#define LORA_MODE_RECV 1

// LORA
#ifndef LoRa_frequency
#define LoRa_frequency 868  // MHz
#endif

/*********************************************************************************
 *                              variable
 *********************************************************************************/
extern GxEPD2_BW<GxEPD2_310_GDEQ031T10, GxEPD2_310_GDEQ031T10::HEIGHT> display;
extern PowersSY6970 PMU;
typedef void (*keypad_cb)(int state, char val);

/*********************************************************************************
 *                              function
 *********************************************************************************/

/**-------------------------------- EPD --------------------------------------**/
void EPD_init(void);
void EPD_start_screen(void);
void EPD_test(const char *name, bool val);
void EPD_test_end(void);
void EPD_loop(void);

/**------------------------------ Keyboard -----------------------------------**/
bool keypad_init(int address);
void keypad_loop(void);
void keypad_regetser_cb(keypad_cb cb);
int keypad_get_val(char *c);

/**------------------------------- Touch -------------------------------------**/
bool touch_init(int addr);
void touch_get_point(void);
void touch_get_xy(int16_t *get_x, int16_t *get_y);
int touch_ui_get_menu_area(void);
int touch_ui_back_menu(void);
bool touch_chk_area(int16_t x, int16_t y, int16_t w, int16_t h);

/**----------------------------- LTR_553ALS ----------------------------------**/
bool LTR_553ALS_init(int i2c_addr);
uint16_t LTR_553ALS_get_channel(int ch);
uint16_t LTR_553ALS_get_ps(void);

/**----------------------------- Gyroscope -----------------------------------**/
bool Gyroscope_init(int i2c_addr);
void Gyroscope_get_val(int val_type, float *x, float *y, float *z);

/**-------------------------------- PMU --------------------------------------**/
bool PMU_init(int addr);
void PMU_loop(void);

/**------------------------------- LORA --------------------------------------**/
bool lora_init(void);
void lora_loop(void);
int  lora_get_mode(void);
void lora_switch_mode(int mode);
bool lora_receive(String *str);
void lora_transmit(const char *str);

/**-------------------------------- GSP --------------------------------------**/
bool GPS_init(void);
void GPS_loop(void);
void GPS_get(float *lat, float *lon, float *speed, float *alt, float *accuracy,
             int *vsat,  int *usat,  int *year,    int *month, int *day,
             int *hour,  int *min,   int *sec);

bool A7682E_init(void);
void A7682E_loop(void);
/**------------------------------ SD card -------------------------------------**/
bool sd_card_init(void);
uint64_t sd_card_get_size(void);

/**------------------------------- Other --------------------------------------**/
bool motor_set(int sta);
int motor_get_sta(void);
bool keypad_led_set(int sta);
int keypad_get_led_sta(void);