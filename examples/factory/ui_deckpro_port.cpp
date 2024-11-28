
#include "Arduino.h"
#include "ui_deckpro_port.h"
#include "factory.h"
#include "utilities.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <TinyGPS++.h>
#include "peripheral.h"
#include "WiFi.h"
#include <ctype.h>
#include <TouchDrvCSTXXX.hpp>

// The TinyGPS++ object
TinyGPSPlus gps;

// extern 
extern TouchDrvCSTXXX touch;


volatile int default_language = DEFAULT_LANGUAGE_EN;
volatile bool default_keypad_light = false;
volatile bool default_motor_status = false;
volatile bool default_gps_status = true;
volatile bool default_lora_status = true;
volatile bool default_gyro_status = true;
volatile bool default_a7682_status = true;
// ----

void ui_disp_full_refr(void)
{
    disp_full_refr();
}
//************************************[ screen 0 ]****************************************** menu
//************************************[ screen 1 ]****************************************** lora

float ui_lora_get_freq(void)
{
    return LORA_FREQ;
}
int ui_lora_get_mode(void)
{
    return lora_get_mode();
}
void ui_lora_set_mode(int mode)
{
    lora_set_mode(mode);
}
void ui_lora_send(const char *str)
{
    lora_transmit(str);
}
void ui_lora_recv_loop(void)
{
    lora_receive_loop();
}
bool ui_lora_get_recv(const char **str, int *rssi)
{
    return lora_get_recv(str, rssi);
}
void ui_lora_set_recv_flag(void)
{
    lora_set_recv_flag();
}
//************************************[ screen 2 ]****************************************** setting
#if 1
// set function
// DEFAULT_LANGUAGE_CN、DEFAULT_LANGUAGE_EN
void ui_setting_set_language(int language)
{
    default_language = language;
}
void ui_setting_set_keypad_light(bool on)
{
    digitalWrite(BOARD_KEYBOARD_LED, on);
    default_keypad_light = on;
}
void ui_setting_set_motor_status(bool on)
{
    digitalWrite(BOARD_MOTOR_PIN, on);
    default_motor_status = on;
}
void ui_setting_set_gps_status(bool on)
{
    // enable GPS module power
    digitalWrite(BOARD_GPS_EN, on);
    default_gps_status = on;
}
void ui_setting_set_lora_status(bool on)
{
    // enable LORA module power
    digitalWrite(BOARD_LORA_EN, on);
    default_lora_status = on;
}
void ui_setting_set_gyro_status(bool on)
{
    // enable gyroscope module power
    digitalWrite(BOARD_1V8_EN, on);
    default_gyro_status = on;
}
void ui_setting_set_a7682_status(bool on)
{
    // enable 7682 module power
    digitalWrite(BOARD_6609_EN, on);
    digitalWrite(BOARD_A7682E_PWRKEY, on);
    default_a7682_status = on;
}

// get function
int ui_setting_get_language(void)
{
    return default_language;
}
bool ui_setting_get_keypad_light(void)
{
    return default_keypad_light;
}
bool ui_setting_get_motor_status(void)
{
    return default_motor_status;
}
bool ui_setting_get_gps_status(void)
{
    return default_gps_status;
}
bool ui_setting_get_lora_status(void)
{
    return default_lora_status;
}
bool ui_setting_get_gyro_status(void)
{
    return default_gyro_status;
}
bool ui_setting_get_a7682_status(void)
{
    return default_a7682_status;
}
#endif
//************************************[ screen 3 ]****************************************** GPS
void ui_GPS_print_info(void)
{
    while (SerialGPS.available())
    {
        Serial.write(SerialGPS.read());
    }
    while (Serial.available())
    {
        SerialGPS.write(Serial.read());
    }
}
void ui_GPS_get_info(float *lat, float *lon, float *speed, float *alt, float *accuracy,
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

//************************************[ screen 4 ]****************************************** Wifi Scan
int is_chinese_utf8(const char *str) {
    unsigned char c = (unsigned char)str[0];
    return (c >= 0xE0 && c <= 0xEF);  // 检查第一个字节是否在 UTF-8 的中文字符范围内
}

void ui_wifi_get_scan_info(ui_wifi_scan_info_t *list, int list_len)
{
    int n = WiFi.scanNetworks();
    if(n > list_len)
        n = list_len;
    
    memset(list, 0, (sizeof(*list) * list_len));
    for(int i = 0; i < n; i++)
    {
        const char *str = WiFi.SSID(i).c_str();
        if(is_chinese_utf8(str))
            continue;
        strncpy(list[i].name, WiFi.SSID(i).c_str(), 16);
        list[i].rssi = WiFi.RSSI(i);
    }
}
//************************************[ screen 5 ]****************************************** State
bool ui_state_get(int peri_id)
{
    return peri_init_st[peri_id];
} 

//************************************[ screen 6 ]****************************************** Battery
// BQ25896
bool ui_batt_25896_is_chg(void)
{
    if(PPM.isVbusIn() == false) {
        return false;
    } else {
        return true;
    }
    // return true;
}
float ui_batt_25896_get_vbus(void)
{
    return (PPM.getVbusVoltage() *1.0 / 1000.0 );
    // return 4.5;
}
float ui_batt_25896_get_vsys(void)
{
    return (PPM.getSystemVoltage() * 1.0 / 1000.0);
    // return 4.5;
}
float ui_batt_25896_get_vbat(void)
{
    return (PPM.getBattVoltage() * 1.0 / 1000.0);
    // return 4.5;
}
float ui_batt_25896_get_volt_targ(void)
{
    return (PPM.getChargeTargetVoltage() * 1.0 / 1000.0);
    // return 4.5; 
}
float ui_batt_25896_get_chg_curr(void)
{
    return (PPM.getChargeCurrent());
    // return 4.5;
}
float ui_batt_25896_get_pre_curr(void)
{
    return (PPM.getPrechargeCurr());;
    // return 4.5;
}
const char * ui_batt_25896_get_chg_st(void)
{
    return PPM.getChargeStatusString();
    // return "hello";
}
const char * ui_batt_25896_get_vbus_st(void)
{
    return PPM.getBusStatusString();
    // return "hello";
}
const char * ui_batt_25896_get_ntc_st(void)
{
    return PPM.getNTCStatusString();
    // return "hello";
}

//************************************[ screen 7 ]****************************************** Input
int ui_input_get_touch_coord(int *x, int *y)
{
    int16_t last_x = 0;
    int16_t last_y = 0;
    int ret = touch.getPoint(&last_x, &last_y);
    *x = last_x;
    *y = last_y;
    return ret;
}

int ui_input_get_keypay_val(char *v)
{
    return keypad_get_val(v);
}

void ui_input_set_keypay_flag(void)
{
    keypad_set_flag();
}
//************************************[ screen 9 ]****************************************** Input

void ui_shutdown_on(void)
{
    PPM.shutdown();
    Serial.println("Shutdown .....");
}