#ifndef __TEST_FACTORY_H__
#define __TEST_FACTORY_H__

/*********************************************************************************
 *                                  INCLUDES
 * *******************************************************************************/
#include <XPowersLib.h>

/*********************************************************************************
 *                                   DEFINES
 * *******************************************************************************/
#define SerialAT Serial1
#define SerialGPS Serial2

#define DISP_REFR_MODE_FULL 0
#define DISP_REFR_MODE_PART 1
/*********************************************************************************
 *                                   MACROS
 * *******************************************************************************/
extern bool flag_sd_init;
extern bool flag_lora_init;
extern bool flag_Touch_init;
extern bool flag_Gyroscope_init;
extern bool flag_Keypad_init;
extern bool flag_BQ25896_init;
extern bool flag_BQ27220_init;
extern bool flag_LTR553ALS_init;

extern XPowersPPM PPM;
/*********************************************************************************
 *                                  TYPEDEFS
 * *******************************************************************************/
union flush_buf_pixel
{
    struct _byte {
        uint8_t b1 : 1;
        uint8_t b2 : 1;
        uint8_t b3 : 1;
        uint8_t b4 : 1;
        uint8_t b5 : 1;
        uint8_t b6 : 1;
        uint8_t b7 : 1;
        uint8_t b8 : 1;
    }bit;
    uint8_t full;
};

/*********************************************************************************
 *                              GLOBAL PROTOTYPES
 * *******************************************************************************/
void disp_full_refr(void); // Next global refresh

#endif