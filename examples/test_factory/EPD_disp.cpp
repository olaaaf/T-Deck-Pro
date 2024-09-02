
#include "factory.h"
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <GxEPD2_BW.h>

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

#define EPD_SCK_PIN  BOARD_EPD_SCK 
#define EPD_MOSI_PIN BOARD_EPD_MOSI
#define EPD_DC_PIN   BOARD_EPD_DC  
#define EPD_CS_PIN   BOARD_EPD_CS  
#define EPD_BUSY_PIN BOARD_EPD_BUSY
#define EPD_RST_PIN  BOARD_EPD_RST 

// macro
#define LINE_MEX_LEN_9PT 22

// extern
extern const unsigned char gImage_menu[9600];
extern bool is_connect_wifi;
extern struct tm timeinfo;
extern bool keypad_update;
extern int keypad_state;

// type
int epd_rotation = 0;
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
GxEPD2_BW<GxEPD2_310_GDEQ031T10, GxEPD2_310_GDEQ031T10::HEIGHT> display(GxEPD2_310_GDEQ031T10(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)); // 
static int timer_tick = 0;
static int coord1[4] = {0};
static int coord2[4] = {0};
static int coord3[4] = {0};

// func
void EPD_init(void)
{
    SPI.begin(EPD_SCK_PIN, -1, EPD_MOSI_PIN, EPD_CS_PIN);
    // display.epd2.selectSPI(SPI, SPISettings(4000000, MSBFIRST, SPI_MODE0));
    display.epd2.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
    u8g2Fonts.begin(display);
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(epd_rotation);
    display.setTextColor(GxEPD_BLACK);
}

void EPD_start_screen(void)
{
    const char logo[] = "Lilygo";
    const char device[] = "T-Deskpro";
    
    int16_t tbx, tby; 
    uint16_t tbw, tbh;
    uint16_t x, y;
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeMonoBold12pt7b);
    display.getTextBounds(logo, 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() - tbw) / 2) - tbx;
    y = ((display.height() - tbh) / 2) - tby;
    display.setFullWindow();
    display.setCursor(x, y);
    display.print(logo);
    display.display(false); // full update

    display.setFont(&FreeMonoBold9pt7b);
    display.getTextBounds(device, 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() - tbw) / 2) - tbx;
    y = y + 30;
    display.setCursor(x, y);
    display.print(device);
    display.display(true); // partial update

    // delay(2000);
}

#if 1 //test
struct EPD_test_item{
    char name[16];
    bool state;
};

struct EPD_test_t{
    struct EPD_test_item item[EDP_TEST_MAX_ITEM];
    int cnt;
};

struct EPD_test_t test_item;

void EPD_test(const char *name, bool val)
{
    int i = test_item.cnt;
    if(i < EDP_TEST_MAX_ITEM){
        strncpy(test_item.item[i].name, name, 16);
        test_item.item[i].state = val;
        test_item.cnt++;
    }
}

bool EPD_get_init_state(const char *name)
{
    for(int i = 0; i < test_item.cnt; i++){
        if(strcmp(test_item.item[i].name, name) == 0){
            return test_item.item[i].state;
        }
    }
    return false;
}

void EPD_test_end(void)
{
    display.fillScreen(GxEPD_WHITE);
    display.drawInvertedBitmap(0, 0, gImage_menu, display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_BLACK);
    display.display(false); // full update
    display.hibernate();
}

#endif

#if 1 //UI
#define EPD_SCREEN_MENU 0

uint16_t EPD_scr_id = 0;
bool EPS_scr_look = false;

void hor_center_pos(int16_t start_y, const GFXfont* f, const char name[])
{
    display.setFont(f);
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;
    // align with centered name
    display.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    // height might be different
    display.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t y = ((start_y) - tbh / 2) - tby; // y is base line!
    // make the window big enough to cover (overwrite) descenders of previous text
    uint16_t wh = (*f).yAdvance;
    uint16_t wy = (start_y) - wh / 2;
    display.setPartialWindow(0, wy, display.width(), wh);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        // display.drawRect(x, y - tbh, tbw, tbh, GxEPD_BLACK);  
        display.setCursor(x, y);
        display.print(name);
    }
    while (display.nextPage());
    display.hibernate();
}

void hor_left_pos(int16_t start_y, const GFXfont* f, const char name[])
{
    display.setFont(f);
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;
    // align with centered name
    display.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    // height might be different
    display.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t y = ((start_y) - tbh / 2) - tby; // y is base line!
    // make the window big enough to cover (overwrite) descenders of previous text
    uint16_t wh = (*f).yAdvance;
    uint16_t wy = (start_y) - wh / 2;
    display.setPartialWindow(0, wy, display.width(), wh);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        // display.drawRect(x, y - tbh, tbw, tbh, GxEPD_BLACK);  
        display.setCursor(0, y);
        display.print(name);
    }
    while (display.nextPage());
    display.hibernate();
}

int display_str_rect(int16_t start_y, int align, const GFXfont* f, const char name[], int *coord)
{
    display.setFont(f);
    display.setTextColor(GxEPD_BLACK);

    int offs = 10;
    int16_t tbx, tby; uint16_t tbw, tbh;
    uint16_t x;
    display.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);
    switch(align){
        case DISPLAY_ALIGN_CENTER: x = ((display.width() - tbw) / 2) - tbx; break;
        case DISPLAY_ALIGN_LEFT:   x = 0;                                   break;
        case DISPLAY_ALIGN_RIGHT:  x = display.width() - tbw;              break;
    }
    uint16_t y = ((start_y) - tbh / 2) - tby; // y is base line!
    uint16_t wh = (*f).yAdvance;
    uint16_t wy = (start_y) - wh / 2;
    display.setPartialWindow(0, wy-offs, display.width(), wh+offs*2);

    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.drawRect(x-offs, y-tbh-offs, tbw+offs*2, tbh+offs*2, GxEPD_BLACK);  
        display.setCursor(x, y);
        display.print(name);
    }
    while (display.nextPage());
    display.hibernate();

    if(coord){
        coord[0] = x-offs;
        coord[1] = y-tbh-offs;
        coord[2] = tbw+offs*2;
        coord[3] = tbh+offs*2;
        Serial.printf("%s:x=%d, y=%d, w=%d, h=%d\n", name, x-offs, y-tbh-offs, tbw+offs*2, tbh+offs*2);
    }
    return wh+offs*2; // h
}

void display_content_clean(void)
{
    int start_y = 60;
    display.fillRect(0, start_y, display.epd2.WIDTH, display.epd2.HEIGHT - start_y, GxEPD_WHITE);
    // display.drawRect(0, start_y, display.epd2.WIDTH, display.epd2.HEIGHT - start_y, GxEPD_BLACK);
    // display.displayWindow(0, start_y, display.epd2.WIDTH, display.epd2.HEIGHT - start_y);
}

void fill_a_line(char *buf, const char str1[], const char str2[], char fill_c)
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int j;

    strncpy(buf, str1, len1);
    for(j = len1; j < LINE_MEX_LEN_9PT -1 -len2; j++) {
        buf[j] = fill_c;
    }
    strncpy(buf + j, str2, len2);
    j = j + len2;
    buf[j] = '\0'; 
}

void show_long_info(int start_y, const GFXfont* f, const char *str)
{
    display.setFont(f);
    if (display.epd2.WIDTH < 104) display.setFont(0);
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(str, 0, 0, &tbx, &tby, &tbw, &tbh);
    // center bounding box by transposition of origin:
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;
    display.setPartialWindow(0, 60, display.width(), display.height());
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(0, start_y);
        display.print(str);
    }while (display.nextPage());
}

// need to use ：display.display(true);
void display_refresh_pos(int16_t start_y, const GFXfont* f, 
                        const char str1[], const char str2[], bool is_full, char fill_c)
{
    char buf[LINE_MEX_LEN_9PT];
    int len1 = 0, len2 = 0;
    int j;

    len1 = strlen(str1);
    if(is_full == true){

        strncpy(buf, str1, len1);

        if(str2 == NULL) {
            for(j = len1; j < LINE_MEX_LEN_9PT-1; j++){
                buf[j] = fill_c;
            }
        }else{
            len2 = strlen(str2);
            for(j = len1; j < LINE_MEX_LEN_9PT -1 -len2; j++){
                buf[j] = fill_c;
            }
            strncpy(buf + j, str2, len2);
            j = j + len2;
        }       
        buf[j] = '\0'; 
    } else {
        display.setFont(f);
        display.setCursor(0, start_y);
        display.println((const char *)str1);
        return;
    }

    display.setFont(f);
    display.setCursor(0, start_y);
    display.println((const char *)buf);
}

void show_curr_clock()
{
    static int tmp = 0;
    char buf[32];

    if(is_connect_wifi == true){
        if (!getLocalTime(&timeinfo)){
            Serial.println("Failed to obtain time");
            return;
        }
        // if(tmp != timeinfo.tm_sec){
        //     tmp = timeinfo.tm_sec;
        //     Serial.println(&timeinfo, "%F %T %A"); // 格式化输出
        //     if(CLOCK_HOUR_FORMAT == CLOCK_12_HOUR_FORMAT){
        //         if(timeinfo.tm_hour > 12)
        //             timeinfo.tm_hour = timeinfo.tm_hour % 12;
        //     }
        //     snprintf(buf, 32, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        //     hor_center_pos(300, &FreeMonoBold9pt7b, buf);
        //     // Alarm: 18:00:00
        //     // if(timeinfo.tm_hour==18 && timeinfo.tm_min==0 && timeinfo.tm_sec==0){
        //     //     int motor = motor_get_sta();
        //     //     motor_set(!motor);
        //     // }
        // }

        if(tmp != timeinfo.tm_min){
            tmp = timeinfo.tm_min;
            Serial.println(&timeinfo, "%F %T %A"); // 格式化输出
            if(CLOCK_HOUR_FORMAT == CLOCK_12_HOUR_FORMAT){
                if(timeinfo.tm_hour > 12)
                    timeinfo.tm_hour = timeinfo.tm_hour % 12;
            }
            snprintf(buf, 32, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
            hor_center_pos(300, &FreeMonoBold9pt7b, buf);
        }

    }else{
        tmp++;
        delay(100);
        snprintf(buf, 32, "tick:%d", tmp);
        hor_center_pos(300, &FreeMono9pt7b, buf);
    }
    display.hibernate();
}

void EPD_screen_menu(void)
{
    display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    display.drawInvertedBitmap(0, 0, gImage_menu, display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_BLACK);
    display.display(false); // full update
    display.hibernate();
    EPS_scr_look = false;
}

void screen_head(const char name[])
{
    int16_t tbx, tby; 
    uint16_t tbw, tbh;
    uint16_t x, y;

    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeMonoBold12pt7b);
    display.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() - tbw) / 2) - tbx;
    y = tbh * 2;
    display.setFullWindow();
    display.setCursor(x, y);
    display.print(name);

    y = y + tbh;
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, y);
    display.println("_____________________");
    display.display(false); // full update
}

void EPD_screen_1(void)
{
    // lora_loop();
    static int cnt = 0, idx = 1;
    int recv_area = 70;
    static int rect_h = 0;

    // Execute once --------------------
    if(EPS_scr_look == false) {
        cnt = 0; idx = 1;

        timer_tick = millis();

        display_content_clean();

        screen_head("LORA");

        if(lora_get_mode() == LORA_MODE_SEND){
            String str = "send freq: ";
            str.concat(LoRa_frequency);
            str += "MHz";
            hor_left_pos(recv_area - 5, &FreeMonoBold9pt7b, str.c_str());
        }
        if(lora_get_mode() == LORA_MODE_RECV) {
            String str = "recv freq: ";
            str.concat(LoRa_frequency);
            str += "MHz";
            hor_left_pos(recv_area - 5, &FreeMonoBold9pt7b, str.c_str());
        }

        rect_h = display_str_rect(290, DISPLAY_ALIGN_CENTER, &FreeMonoBold9pt7b, "Switch Lora Mode", NULL);

        display.hibernate();

        EPS_scr_look = true;
    }
    // Execute once end --------------------

    bool is_clk = touch_chk_area(0, 280, display.width(), 280+rect_h);

    // Serial.printf("touch_chk_area: [%d]\n", 280+rect_h);

    if(is_clk){

        int mode = lora_get_mode();
        if(mode == LORA_MODE_SEND){
            lora_switch_mode(LORA_MODE_RECV);
        } else  if(mode == LORA_MODE_RECV){
            lora_switch_mode(LORA_MODE_SEND);
        }
        EPS_scr_look = false; // unlock
        Serial.printf("Lora mode is switched: [%d]\n", lora_get_mode());
        return;
    }

    if(lora_get_mode() == LORA_MODE_SEND){
        if(millis() - timer_tick > 1000){
            timer_tick = millis();
            String str = "Lora Send! #" + String(cnt++);
            lora_transmit(str.c_str());
            hor_left_pos(recv_area + idx*20, &FreeMono9pt7b, str.c_str());
            if(idx++ > 8){
                idx = 1;
            }
        }
    } else if(lora_get_mode() == LORA_MODE_RECV){
        String str;
        if(lora_receive(&str)){
            hor_left_pos(recv_area + idx*20, &FreeMono9pt7b, str.c_str());
            if(idx++ > 8){
                idx = 1;
            }
        }
    }
}

void EPD_screen_2(void)
{   
    float lat      = 0; // Latitude
    float lon      = 0; // Longitude
    float speed    = 0; // Speed over ground
    float alt      = 0; // Altitude
    float accuracy = 0; // Accuracy
    int   vsat     = 0; // Visible Satellites
    int   usat     = 0; // Used Satellites
    int   year     = 0; // 
    int   month    = 0; // 
    int   day      = 0; // 
    int   hour     = 0; // 
    int   min      = 0; // 
    int   sec      = 0; // 

    int start_y = 100;
    static int updata_cnt = 0;

    char buf[LINE_MEX_LEN_9PT];
    // Execute once --------------------
    if(EPS_scr_look == false) {
        screen_head("GPS"); 

        timer_tick = millis();
        updata_cnt = 0;

        EPS_scr_look = true;
    }
    // Execute once end --------------------

    // pinMode(BOARD_GPS_PPS, INPUT);
    // if(digitalRead(BOARD_GPS_PPS) == HIGH){
    //     digitalWrite(BOARD_KEYBOARD_LED, HIGH);
    // }else if(digitalRead(BOARD_GPS_PPS) == LOW){
    //     digitalWrite(BOARD_KEYBOARD_LED, LOW);
    // }

    if(millis() - timer_tick > 2000){  // Update GPS data every 2 seconds

        timer_tick = millis();

        GPS_loop();

        GPS_get(&lat, &lon, &speed, &alt, &accuracy, &vsat, &usat, &year, &month, &day, &hour, &min, &sec);

        display_content_clean();

        snprintf(buf, LINE_MEX_LEN_9PT, "%s %d", "Use GPS outdoors!", updata_cnt++);
        display_refresh_pos(70, &FreeMonoBold9pt7b, buf, NULL, false, ' ');

        snprintf(buf, LINE_MEX_LEN_9PT, " %f", lat);
        display_refresh_pos(start_y, &FreeMono9pt7b, "lat", buf, true, '.');

        snprintf(buf, LINE_MEX_LEN_9PT, " %f", lon);
        display_refresh_pos(start_y + 30, &FreeMono9pt7b, "lon ", buf, true, '.');

        snprintf(buf, LINE_MEX_LEN_9PT, " %f", speed);
        display_refresh_pos(start_y + 60, &FreeMono9pt7b, "speed ", buf, true, '.');

        snprintf(buf, LINE_MEX_LEN_9PT, " %f", alt);
        display_refresh_pos(start_y + 90, &FreeMono9pt7b, "altitude ", buf, true, '.');

        snprintf(buf, LINE_MEX_LEN_9PT, " %f", accuracy);
        display_refresh_pos(start_y + 120, &FreeMono9pt7b, "accuracy ", buf, true, '.');

        snprintf(buf, LINE_MEX_LEN_9PT, " %d", vsat);
        display_refresh_pos(start_y + 150, &FreeMono9pt7b, "vsats ", buf, true, '.');

        snprintf(buf, LINE_MEX_LEN_9PT, " %d", usat);
        display_refresh_pos(start_y + 180, &FreeMono9pt7b, "usat ", buf, true, '.');

        snprintf(buf, LINE_MEX_LEN_9PT, " %02d.%02d.%02d - %02d:%02d:%02d", year, month, day, hour, min, sec);
        display_refresh_pos(start_y + 210, &FreeMono9pt7b, buf, NULL, false, ' ');

        display.display(true);
        display.hibernate();
    }
}

void EPD_screen_3(void)
{
    int16_t prev_x, prev_y;
    static int16_t touch_x = 0, touch_y = 0;
    char buf_line[32];
    static char buf[64];
    static int keypad_cnt = 0;

    // Execute once --------------------
    if(EPS_scr_look == false) {
        keypad_cnt = 0;
        screen_head("Touch"); 
        EPS_scr_look = true;
    }
    // Execute once end --------------------

    touch_get_xy(&prev_x, &prev_y);
    if((touch_x != prev_x && touch_y != prev_y)  || keypad_update == true){
        touch_x = prev_x;
        touch_y = prev_y;
        keypad_update = false;

        display_content_clean();
        snprintf(buf_line, 32, "%d", touch_x);
        display_refresh_pos(90, &FreeMonoBold9pt7b, "touch X:", buf_line, true, '.');
        snprintf(buf_line, 32, "%d", touch_y);
        display_refresh_pos(130, &FreeMonoBold9pt7b, "touch Y:", buf_line, true, '.');

        char c;
        keypad_get_val(&c);
        snprintf(buf_line, 32, "%c", c);
        display_refresh_pos(170, &FreeMonoBold9pt7b, "keypad:", buf_line, true, '.');

        display_refresh_pos(200, &FreeMonoBold9pt7b, "-", NULL, true, '-');

        Serial.println(c);
        if(keypad_state == KEYPAD_PRESS){
            buf[keypad_cnt] = c;
            buf[keypad_cnt + 1] = '\0';
            keypad_cnt++;
        }
        display_refresh_pos(230, &FreeMonoBold9pt7b, buf, NULL, false, ' ');

        display.display(true); // partial update
        display.hibernate();
    }
}

void EPD_screen_4(void)
{   
    int i = 1;
    char buf[LINE_MEX_LEN_9PT];
    File root = SD.open("/"); // root

    if(EPS_scr_look == false) {
        if(EPD_get_init_state("SD_card") == false){
            screen_head("SD card"); 
            hor_center_pos(display.epd2.HEIGHT / 2, &FreeMonoBold9pt7b, "No SD card!");
            EPS_scr_look = true;
            return;
        }

        screen_head("SD card"); 
        if(!root){
            Serial.println("Failed to open directory");
            return;
        }
        if(!root.isDirectory()){
            Serial.println("Not a directory");
            return;
        }

        snprintf(buf, LINE_MEX_LEN_9PT, "SD Card Size: %lluMB", sd_card_get_size());
        display.setFont(&FreeMono9pt7b);
        display.setCursor(0, 70);
        display.println((const char *)buf);
        display.display(true); // partial update

        File file = root.openNextFile();
        while(file){

            snprintf(buf, LINE_MEX_LEN_9PT, "%s", file.name());
            if(file.isDirectory()){
            }
            else {
                // display.setCursor(0, i * 20 + 70);
                // display.println((const char *)buf);
                // display.display(true); // partial update

                hor_left_pos(i * 20 + 70, &FreeMono9pt7b, buf);
            }
            file = root.openNextFile();
            i++;
            if(i > 12){
                i = 1;
            }
        }
        display.hibernate();
        EPS_scr_look = true;
    }
}

void EPD_screen_5(void)
{
    char buf[LINE_MEX_LEN_9PT];
    char line[16];
    uint16_t start_pos = 0;

    if(EPS_scr_look == false) {
        screen_head("Test"); 

        for(int i = 0; i < test_item.cnt; i++){
            uint16_t str_len = strlen(test_item.item[i].name);
            uint16_t tmp = test_item.item[i].state ? strlen("OK") : strlen("Fail");
            str_len += tmp + 2; // add two Spaces

            int j;
            for(j = 0; j < LINE_MEX_LEN_9PT-1 - str_len; j++){
                line[j] = '-';
            }
            line[j] = '\0';

            snprintf(buf, LINE_MEX_LEN_9PT, "%s %s %s", test_item.item[i].name, line, test_item.item[i].state ? "OK" : "Fail");

            if(start_pos >= 7){
                start_pos = 0;
                delay(1000);
            }

            hor_center_pos(start_pos * 35 + 77, &FreeMonoBold9pt7b, buf);

            start_pos++;
        }
        display.hibernate();
        EPS_scr_look = true;
    }
}

void EPD_screen_6(void)
{
    char buf[LINE_MEX_LEN_9PT];
    uint16_t ch0 = LTR_553ALS_get_channel(0);
    uint16_t ch1 = LTR_553ALS_get_channel(1);
    uint16_t ps = LTR_553ALS_get_ps();
    int start_pos = 70;

    // Execute once --------------------
    if(EPS_scr_look == false) {
        timer_tick = millis();

        screen_head("Sensor"); 
        
        EPS_scr_look = true;
    }
    // Execute once end --------------------

    if(millis() - timer_tick > 200){
        timer_tick = millis();

        // Light sensor
        display_content_clean();

        display_refresh_pos(start_pos, &FreeMonoBold9pt7b, "Light sensor->", NULL, false, ' ');
        snprintf(buf, LINE_MEX_LEN_9PT, " c0:%03d c1:%03d ps:%03d", ch0, ch1, ps);
        display_refresh_pos(start_pos+20, &FreeMono9pt7b, buf, NULL, false, ' ');

        // Gyroscope
        float x=0, y=0, z=0;
        Gyroscope_get_val(2, &x, &y, &z);
        display_refresh_pos(start_pos+50, &FreeMonoBold9pt7b, "Gyroscope->", NULL, false, ' ');
        snprintf(buf, LINE_MEX_LEN_9PT, "%.3f", x);
        display_refresh_pos(start_pos+70, &FreeMono9pt7b, "gyros_x:", buf, true, ' ');
        snprintf(buf, LINE_MEX_LEN_9PT, "%.3f", y);
        display_refresh_pos(start_pos+85, &FreeMono9pt7b, "gyros_y:", buf, true, ' ');
        snprintf(buf, LINE_MEX_LEN_9PT, "%.3f", z);
        display_refresh_pos(start_pos+100, &FreeMono9pt7b, "gyros_z:", buf, true, ' ');

        // PMU SY6970
        display_refresh_pos(start_pos+130, &FreeMonoBold9pt7b, "PMU[SY6970]->", NULL, false, ' ');
        int charge_state = PMU.chargeStatus();
        switch (charge_state) {
            case 0: snprintf(buf, LINE_MEX_LEN_9PT, "%s", "Not"); break;
            case 1: snprintf(buf, LINE_MEX_LEN_9PT, "%s", "Pre"); break;
            case 2: snprintf(buf, LINE_MEX_LEN_9PT, "%s", "Fast"); break;
            case 3: snprintf(buf, LINE_MEX_LEN_9PT, "%s", "Done"); break;
            default: break;
        }
        display_refresh_pos(start_pos+150, &FreeMono9pt7b, "charg:", buf, true, ' ');
        snprintf(buf, LINE_MEX_LEN_9PT, "%dmA", PMU.getChargerConstantCurr());
        display_refresh_pos(start_pos+165, &FreeMono9pt7b, "curr:", buf, true, ' ');
        snprintf(buf, LINE_MEX_LEN_9PT, "%dmV", PMU.getBattVoltage());
        display_refresh_pos(start_pos+180, &FreeMono9pt7b, "VBAT:", buf, true, ' ');
        snprintf(buf, LINE_MEX_LEN_9PT, "%dmV", PMU.getSystemVoltage());
        display_refresh_pos(start_pos+195, &FreeMono9pt7b, "VSYS:", buf, true, ' ');

        // Touch
        // int16_t touch_x = 0, touch_y = 0;
        // touch_get_xy(&touch_x, &touch_y);
        // display_refresh_pos(start_pos+195, &FreeMonoBold9pt7b, "Touch->", NULL, false, ' ');
        // snprintf(buf, LINE_MEX_LEN_9PT, "%d", touch_x);
        // display_refresh_pos(start_pos+215, &FreeMono9pt7b, "touch_x:", buf, true, ' ');
        // snprintf(buf, LINE_MEX_LEN_9PT, "%d", touch_y);
        // display_refresh_pos(start_pos+230, &FreeMono9pt7b, "touch_y:", buf, true, ' ');

        display.display(true); // partial update
        display.hibernate();
    }
}

void EPD_screen_7(void)
{
    static int A7682E_cnt = 0;
    char buf[LINE_MEX_LEN_9PT];

    if(EPS_scr_look == false) {
        A7682E_cnt = 0;
        timer_tick = millis();
        screen_head("A7682E"); 
        EPS_scr_look = true;
    }
    
    if(millis() - timer_tick > 2000){  // Update GPS data every 2 seconds

        timer_tick = millis();

        display_content_clean();

        snprintf(buf, LINE_MEX_LEN_9PT, "%s %d", "Use GPS outdoors!", A7682E_cnt++);
        display_refresh_pos(70, &FreeMonoBold9pt7b, buf, NULL, false, ' ');

        display.display(true);
        display.hibernate();
    }
    A7682E_loop();
}

void EPD_screen_8(void)
{
    static bool bott_sta = false;
    
    // Execute once --------------------
    if(EPS_scr_look == false) {
        screen_head("Other"); 
        display_str_rect(80, DISPLAY_ALIGN_CENTER, &FreeMonoBold9pt7b, "Bottom 1", coord1);
        display_str_rect(160, DISPLAY_ALIGN_CENTER, &FreeMonoBold9pt7b, "Shake SWITCH BTN", coord2);
        display_str_rect(240, DISPLAY_ALIGN_CENTER, &FreeMonoBold9pt7b, "LED SWITCH BTN", coord3);
        EPS_scr_look = true;
    }
    // Execute once end --------------------

    bool is_clk1 = touch_chk_area(coord1[0], coord1[1], coord1[2], coord1[3]);
    if(is_clk1){
        Serial.printf("%s:x=%d, y=%d, w=%d, h=%d\n", "Bottom-1", coord1[0], coord1[1], coord1[2], coord1[3]);
        bott_sta = !bott_sta;
        if(bott_sta == true){
            display_str_rect(80, DISPLAY_ALIGN_CENTER, &FreeMonoBold9pt7b, "Bottom 1 ON", NULL);
        } else if(bott_sta == false){
            display_str_rect(80, DISPLAY_ALIGN_CENTER, &FreeMonoBold9pt7b, "Bottom 1 OFF", NULL);
        }
    }

    bool is_clk2 = touch_chk_area(coord2[0], coord2[1], coord2[2], coord2[3]);
    if(is_clk2){
        Serial.printf("%s:x=%d, y=%d, w=%d, h=%d\n", "Bottom-2", coord2[0], coord2[1], coord2[2], coord2[3]);
        int motor = motor_get_sta();
        motor_set(!motor);
        if(motor == LOW){
            display_str_rect(160, DISPLAY_ALIGN_CENTER, &FreeMonoBold9pt7b, "Shake SWITCH ON", NULL);
        } else if(motor == HIGH){
            display_str_rect(160, DISPLAY_ALIGN_CENTER, &FreeMonoBold9pt7b, "Shake SWITCH OFF", NULL);
        }
    }

    bool is_clk3 = touch_chk_area(coord3[0], coord3[1], coord3[2], coord3[3]);
    if(is_clk3){
        Serial.printf("%s:x=%d, y=%d, w=%d, h=%d\n", "Bottom-3", coord3[0], coord3[1], coord3[2], coord3[3]);
        int led = keypad_get_led_sta();
        keypad_led_set(!led);
        if(led == LOW){
            display_str_rect(240, DISPLAY_ALIGN_CENTER, &FreeMonoBold9pt7b, "LED SWITCH ON", NULL);
        } else if(led == HIGH){
            display_str_rect(240, DISPLAY_ALIGN_CENTER, &FreeMonoBold9pt7b, "LED SWITCH OFF", NULL);
        }
    }

    display.hibernate();
}

void EPD_screen_9(void)
{
    static int help_page = 0;
    int sum_page = 3;
    char buf[LINE_MEX_LEN_9PT];
    // Execute once --------------------
    if(EPS_scr_look == false) {
        screen_head("About"); 

        if(help_page == 0){
            // hor_left_pos(70, &FreeMonoBold9pt7b, "This is page 1, hellohellohellohellohellohello");
            show_long_info(70, &FreeMono9pt7b,  "     System info     \n"
                                                "                     \n"
                                                "Name ..... T-Desk-pro\n"
                                                "IDF .......... v4.4.4\n"
                                                "Mode ....... ESP32-S3\n"
                                                "ID .......... 2979932\n"
                                                "MAC ....... 771305520\n"
                                                "Cores ............. 2\n"
                                                "Memory ......... 16MB\n"
                                                "Frimware ....... v1.0\n");
        } else if (help_page == 1){
            show_long_info(70, &FreeMono9pt7b,  "-------- GPS --------\n"
                                                "When using GPS, please put the board outside "
                                                "the house to capture satellite signals, because" 
                                                "there will be no GPS signal indoors due to occlusion.\n"
                                                "                     \n"
                                                "---- About return ---\n"
                                                "Please click on the top title to return.");
        } else if (help_page == 2){
            show_long_info(70, &FreeMono9pt7b,  "------ A7682E -------\n"
                                                "A7682E does not have a built-in GPS module; "
                                                );
        } else if (help_page == 3) {
            show_long_info(70, &FreeMono9pt7b,  "------ Antenna ------\n"
                                                "When using functions related to wireless "
                                                "communication such as Lora, GPS, and WiFi, "
                                                "please insert the antenna of the corresponding "
                                                "function to obtain more stable communication.");
        }

        snprintf(buf, LINE_MEX_LEN_9PT, "NEXT PAGE %d-%d", help_page, sum_page);
        display_str_rect(295, DISPLAY_ALIGN_CENTER, &FreeMonoBold9pt7b, buf, coord1);

        EPS_scr_look = true;
    }
    // Execute once end --------------------

    bool is_clk1 = touch_chk_area(coord1[0], coord1[1], coord1[2], coord1[3]);
    if(is_clk1){
        Serial.printf("%s:x=%d, y=%d, w=%d, h=%d [%d]\n", "SHOW-NEXT-PAGE", coord1[0], coord1[1], coord1[2], coord1[3], help_page);
        if(help_page++ >= sum_page){
            help_page = 0;
        }
        EPS_scr_look = false;
    }

    display.hibernate();
}

void EPD_loop(void)
{
    static int led = 0;

    if(EPD_scr_id == EPD_SCREEN_MENU){
        int id = touch_ui_get_menu_area();
        EPD_scr_id = id;
        
        if(id != 0){
            Serial.printf("---------------[%d]---------------\n", id);
            display.hibernate();
            Serial.printf("sleep1\n");
        }

        if(id == 0) 
            show_curr_clock();
    }

    // back to menu
    if(EPD_scr_id != EPD_SCREEN_MENU){
        int ret = 0;
        ret = touch_ui_back_menu();
        if(ret){
            EPD_scr_id = EPD_SCREEN_MENU;
            EPD_screen_menu();
            return;
        }
    }

    // entry screen
    switch (EPD_scr_id) {
        case 1: EPD_screen_1(); break;
        case 2: EPD_screen_2(); break;
        case 3: EPD_screen_3(); break;
        case 4: EPD_screen_4(); break;
        case 5: EPD_screen_5(); break;
        case 6: EPD_screen_6(); break;
        case 7: EPD_screen_7(); break;
        case 8: EPD_screen_8(); break;
        case 9: EPD_screen_9(); break;
        default:
            break;
    }
}
#endif
