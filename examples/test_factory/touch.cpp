#include "test_factory.h"


#include <TouchDrvCSTXXX.hpp>
TouchDrvCSTXXX touch;


bool touch_init(int addr)
{
    touch.setPins(BOARD_TOUCH_RST, BOARD_TOUCH_INT);
    bool hasTouch = touch.begin(Wire, addr, BOARD_TOUCH_SDA, BOARD_TOUCH_SCL);
    if (!hasTouch) {
        Serial.println("Failed to find Capacitive Touch !");
        return false;
    } else {
        Serial.println("Find Capacitive Touch");
        return true;
    }
}

int16_t touch_x = 0, touch_y = 0;
bool touch_update_flag = false;
void touch_get_point(void)
{
    int16_t read_x, read_y;
    uint8_t touched = touch.getPoint(&read_x, &read_y, 1);
    if(read_x != touch_x && read_y != touch_y){
        touch_x = read_x;
        touch_y = read_y;
        Serial.print("touch_x=");Serial.print(touch_x);
        Serial.print(", touch_y=");Serial.println(touch_y);
        touch_update_flag = true;
    }
   
    // if (touched) {
    //     Serial.print("touch_x=");Serial.print(touch_x);
    //     Serial.print(", touch_y=");Serial.println(touch_y);
    // }
}

void touch_get_xy(int16_t *get_x, int16_t *get_y)
{
    if(get_x != NULL && get_y != NULL){
        *get_x = touch_x;
        *get_y = touch_y;
    }
}

/*------------------------------ EPD touch ------------------------------*/
int touch_ui_get_menu_area(void)
{
    uint16_t icon_size = 50;
    uint16_t icon_hor_size = 70;
    uint16_t icon_ver_size = 88;
    uint16_t start_x = 25;
    uint16_t start_y = 28;

    if(touch_update_flag == false){
        return 0;
    }

    int row=-1, col=-1;
    for(int i = 0; i < 3; i++){
        uint16_t x1 = start_x + i * icon_hor_size;
        uint16_t x2 = x1 + icon_size;
        if(touch_x >= x1 && touch_x <= x2){
            row = i;
            break;
        }
    }

    for(int j = 0; j < 3; j++){
        uint16_t y1 = start_y + j * icon_ver_size;
        uint16_t y2 = y1 + icon_size;
        if(touch_y >= y1 && touch_y <= y2){
            col = j;
            break;
        }
    }

    if(row != -1 && col != -1){
        touch_update_flag = false;
        return (col * 3 + row) + 1; // 1-9
    }

    return 0;
}

int touch_ui_back_menu(void)
{
    if(touch_update_flag == false){
        return 0;
    }

    if(touch_x < 240 && touch_y < 40){
        touch_update_flag = false;
        return 1;
    }
    
    return 0;
}

bool touch_chk_area(int16_t x, int16_t y, int16_t w, int16_t h)
{
    if(touch_update_flag == false){
        
        return false;
    }

    if(touch_x > x && touch_x < x+w && touch_y > y && touch_y < y+h) {
        touch_update_flag = false;
        return true;
    }

    return false;
}

