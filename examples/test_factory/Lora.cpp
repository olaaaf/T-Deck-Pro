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
    int state = radio.begin();
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        log_e("LORA failed, code %d", state);
        Serial.print(F("failed, code "));
        Serial.println(state);
        return false;
    }

    // set carrier frequency to 433.5 MHz
    if (radio.setFrequency(868.0) == RADIOLIB_ERR_INVALID_FREQUENCY) {
        Serial.println(F("Selected frequency is invalid for this module!"));
        while (true);
    }

    // set bandwidth to 250 kHz
    if (radio.setBandwidth(125.0) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
        Serial.println(F("Selected bandwidth is invalid for this module!"));
        while (true);
    }

    // set spreading factor to 12
    if (radio.setSpreadingFactor(10) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
        Serial.println(F("Selected spreading factor is invalid for this module!"));
        while (true);
    }

    // set coding rate to 6
    if (radio.setCodingRate(6) == RADIOLIB_ERR_INVALID_CODING_RATE) {
        Serial.println(F("Selected coding rate is invalid for this module!"));
        while (true);
    }

    // set LoRa sync word to 0xAB
    if (radio.setSyncWord(0xAB) != RADIOLIB_ERR_NONE) {
        Serial.println(F("Unable to set sync word!"));
        while (true);
    }

    // set output power to 10 dBm (accepted range is -17 - 22 dBm)
    if (radio.setOutputPower(22) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        Serial.println(F("Selected output power is invalid for this module!"));
        while (true);
    }

    // set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
    // NOTE: set value to 0 to disable overcurrent protection
    if (radio.setCurrentLimit(140) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
        Serial.println(F("Selected current limit is invalid for this module!"));
        while (true);
    }

    // set LoRa preamble length to 15 symbols (accepted range is 0 - 65535)
    if (radio.setPreambleLength(15) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
        Serial.println(F("Selected preamble length is invalid for this module!"));
        while (true);
    }

    // disable CRC
    if (radio.setCRC(false) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
        Serial.println(F("Selected CRC is invalid for this module!"));
        while (true);
    }

    // Some SX126x modules have TCXO (temperature compensated crystal
    // oscillator). To configure TCXO reference voltage,
    // the following method can be used.
    if (radio.setTCXO(2.4) == RADIOLIB_ERR_INVALID_TCXO_VOLTAGE) {
        Serial.println(F("Selected TCXO voltage is invalid for this module!"));
        while (true);
    }

    // Some SX126x modules use DIO2 as RF switch. To enable
    // this feature, the following method can be used.
    // NOTE: As long as DIO2 is configured to control RF switch,
    //       it can't be used as interrupt pin!
    if (radio.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE) {
        Serial.println(F("Failed to set DIO2 as RF switch!"));
        while (true);
    }

    Serial.println(F("All settings succesfully changed!"));

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