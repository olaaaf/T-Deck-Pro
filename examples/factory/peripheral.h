#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

enum {
    E_PERI_LORA = 0,
    E_PERI_KYEPAD,
    E_PERI_BQ25896,
    E_PERI_BQ27220,
    E_PERI_GPS,
    E_PERI_BHI260AP,
    E_PERI_INK_SCREEN,
    E_PERI_LTR_553ALS,
    E_PERI_TOUCH,
    E_PERI_MIC,
    E_PERI_SD,
    E_PERI_A7683E,
    E_PERI_PCM5102A,
    E_PERI_NUM_MAX,
};

// lora sx1262
#define LORA_FREQ      850.0
#define LORA_MODE_SEND 0
#define LORA_MODE_RECV 1

bool lora_init(void);
void lora_set_mode(int mode);
int lora_get_mode(void);
void lora_receive_loop(void);
void lora_transmit(const char *str);
bool lora_get_recv(const char **str, int *rssi);
void lora_set_recv_flag(void);

// keypad
#define KEYPAD_PRESS   1
#define KEYPAD_RELEASE 0

typedef void (*keypad_cb)(int state, char val);

bool keypad_init(int address);
int keypad_get_val(char *c);
void keypad_loop(void);
void keypad_regetser_cb(keypad_cb cb);
void keypad_set_flag(void);
#endif