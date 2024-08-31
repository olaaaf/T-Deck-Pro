

#include "factory.h"
#include <WiFi.h>

// wifi
const char* ssid = "xinyuandianzi";
const char* password = "AA15994823428";
const char *ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
bool is_connect_wifi = false;
struct tm timeinfo;
static uint32_t last_tick;
bool has_display = false;

void wifi_init(void)
{
    WiFi.begin(ssid, password);
    wl_status_t wifi_state = WiFi.status();
    last_tick = millis();
    while (wifi_state != WL_CONNECTED){
        delay(500);
        Serial.print(".");
        wifi_state = WiFi.status();
        if(wifi_state == WL_CONNECTED){
            is_connect_wifi = true;
            Serial.println("WiFi connected!");
            configTime(8 * 3600, 0, ntpServer1, ntpServer2);
            break;
        }
        if (millis() - last_tick > 5000) {
            Serial.println("WiFi connected falied!");
            last_tick = millis();
            break;
        }
    }
}

void setup()
{
    Serial.begin(115200);
    
    // int start_delay = 2;
    // while (start_delay) {
    //     Serial.print(start_delay);
    //     delay(1000);
    //     start_delay--;
    // }

    // wifi_init();
    
    // i2c devices
    byte error, address;
    int nDevices = 0;
    Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);
    for(address = 0x01; address < 0x7F; address++){
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if(error == 0){ // 0: success.
            nDevices++;
            log_i("I2C device found at address 0x%x\n", address);
        }
    }

    // SPI
    SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI, -1);

    // IO
    pinMode(BOARD_KEYBOARD_LED, OUTPUT);
    pinMode(BOARD_MOTOR_PIN, OUTPUT);
    pinMode(BOARD_6609_EN, OUTPUT);
    pinMode(BOARD_LORA_EN, OUTPUT);
    pinMode(BOARD_GPS_EN, OUTPUT); 
    pinMode(BOARD_1V8_EN, OUTPUT); 
    pinMode(BOARD_A7682E_PWRKEY, OUTPUT); 

    digitalWrite(BOARD_KEYBOARD_LED, LOW);
    digitalWrite(BOARD_MOTOR_PIN, LOW);
    digitalWrite(BOARD_6609_EN, HIGH);
    digitalWrite(BOARD_LORA_EN, HIGH);
    digitalWrite(BOARD_GPS_EN, HIGH);
    digitalWrite(BOARD_1V8_EN, HIGH);
    digitalWrite(BOARD_A7682E_PWRKEY, HIGH);

    has_display = touch_init(BOARD_I2C_ADDR_TOUCH);
    if(has_display){
        // EPD display
        EPD_init();
        EPD_start_screen();
        // Test UI
        EPD_test("Touch",      has_display);
        EPD_test("Keypad",     keypad_init(BOARD_I2C_ADDR_KEYBOARD));
        EPD_test("LTR_553ALS", LTR_553ALS_init(BOARD_I2C_ADDR_LTR_553ALS));
        EPD_test("Gyroscope",  Gyroscope_init(BOARD_I2C_ADDR_GYROSCOPDE));
        EPD_test("PMU",        PMU_init(BOARD_I2C_ADDR_PMU_SY6970));
        EPD_test("LORA",       lora_init());
        EPD_test("SD_card",    sd_card_init());
        EPD_test("2.GPS",      GPS_init());
        // EPD_test("2.A7682E",   A7682E_init());
        // entry UI
        EPD_test_end();
    } else {
        keypad_init(BOARD_I2C_ADDR_KEYBOARD);
        LTR_553ALS_init(BOARD_I2C_ADDR_LTR_553ALS);
        Gyroscope_init(BOARD_I2C_ADDR_GYROSCOPDE);
        PMU_init(BOARD_I2C_ADDR_PMU_SY6970);
        lora_init();
        sd_card_init();
        GPS_init();
        // A7682E_init();
    }
}

void GetSerialCmd() {
    String inStr = ""; 
    while(Serial.available()) { 
        inStr += (char)Serial.read();  
    }

    if(inStr != ""){
        Serial.print("input: ");
        Serial.println(inStr);

        switch ((byte)inStr[0]) {
            case 'L': // Led on
                digitalWrite(BOARD_KEYBOARD_LED, HIGH);
                break;
            case 'l': // Led off
                digitalWrite(BOARD_KEYBOARD_LED, LOW);
                break;
            case 'M': // Motor on
                motor_set(HIGH);
                break;
            case 'm': // Motor off
                motor_set(LOW);
                break;
            default: break;
        }
    }
}

uint32_t timer_tick = 0;
uint32_t wifi_tick = 0;
uint32_t lora_cnt = 0;
void loop()
{
    // GetSerialCmd();

    touch_get_point();

    keypad_loop();

    if(has_display){
        EPD_loop();
    }
    

    delay(1);

    // if(millis() - timer_tick > 1000){
    //     timer_tick = millis();
    //     String str = "Lora Send! #" + String(lora_cnt++);
    //     lora_transmit(str.c_str());
    //     Serial.println(str);

    //     motor_set(!motor_get_sta());
    // }

    // if(millis() - wifi_tick > 3000){
    //     wifi_tick = millis();
    //     wifi_scan();
    // }
}

// other function
int motor_sta = LOW;
bool motor_set(int sta) // sta: LOW/HIGH
{
    digitalWrite(BOARD_MOTOR_PIN, sta);
    motor_sta = sta;
    return (bool)sta;
}

int motor_get_sta(void)
{
    return motor_sta;
}

int keypad_led_sta = LOW;
bool keypad_led_set(int sta)
{
    digitalWrite(BOARD_KEYBOARD_LED, sta);
    keypad_led_sta = sta;
    return (bool)sta;
}

int keypad_get_led_sta(void)
{
    return keypad_led_sta;
}


