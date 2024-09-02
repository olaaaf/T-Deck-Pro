#include "test_factory.h"
#include <RadioLib.h>

SX1262 radio = new Module(BOARD_LORA_CS, BOARD_LORA_INT, BOARD_LORA_RST, BOARD_LORA_BUSY);

static int lora_mode = LORA_MODE_SEND;

// transmit 
int transmissionState = RADIOLIB_ERR_NONE;
volatile bool transmittedFlag = false;

void set_transmit_flag(void){
    transmittedFlag = true;
}

// receive
int receivedState = RADIOLIB_ERR_NONE;
volatile bool receivedFlag = false;

void set_receive_flag(void){
    receivedFlag = true;
}


bool lora_init(void)
{
    // int state = radio.begin(LoRa_frequency);
    int state = radio.beginFSK(868.0);
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(F("success!"));
        // return true;
    }
    else
    {
        log_e("LORA failed, code %d", state);
        Serial.print(F("failed, code "));
        Serial.println(state);
        // return false;
    }

    if(lora_mode == LORA_MODE_SEND){
        radio.setPacketSentAction(set_transmit_flag);
        Serial.println(F("[SX1262] Sending first packet ... "));
        transmissionState = radio.startTransmit("Hello World!");
    } else if(lora_mode == LORA_MODE_RECV){
        radio.setPacketReceivedAction(set_receive_flag);
        Serial.println(F("[SX1262] Starting to listen ... "));
        receivedState = radio.startReceive();
        if(receivedState == RADIOLIB_ERR_NONE){
            Serial.println(F("success!"));
        }else{
            Serial.print(F("failed, code "));
            Serial.println(receivedState);
            while (true);
        }
    }
    return true;
}

void lora_loop(void)
{
}

int lora_get_mode(void)
{
    return lora_mode;
}

/**
 * mode : LORA_MODE_SEND / LORA_MODE_RECV
 */
void lora_switch_mode(int mode)
{
    if(mode == LORA_MODE_SEND){
        radio.setPacketSentAction(set_transmit_flag);
        Serial.println(F("[LORA] Sending first packet ... "));
        transmissionState = radio.startTransmit("Hello World!");
    } else if(mode == LORA_MODE_RECV){
        radio.setPacketReceivedAction(set_receive_flag);
        Serial.println(F("[LORA] Starting to listen ... "));
        receivedState = radio.startReceive();
    }
    lora_mode = mode;
}

bool lora_receive(String *str)
{
    bool ret = false;
    if(receivedFlag){
        receivedFlag = false;
        // String str;
        receivedState = radio.readData(*str);
        if(receivedState == RADIOLIB_ERR_NONE){
            Serial.print(F("[SX1262] Data:\t\t"));
            // Serial.println(str);
            ret = true;
        }else{
            Serial.print(F("failed, code "));
            Serial.println(receivedState);
        }
    }
    return ret;
}

void lora_transmit(const char *str)
{
    if(transmittedFlag){
        transmittedFlag = false;
        if(transmissionState == RADIOLIB_ERR_NONE){
            Serial.println(F("transmission finished!"));
        } else {
            Serial.print(F("failed, code "));
            Serial.println(transmissionState);
        }

        radio.finishTransmit();
        Serial.print(F("[Lora] Sending another packet ... "));
        transmissionState = radio.startTransmit(str);
    }
}