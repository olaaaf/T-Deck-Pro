

#ifndef BQ27227_H
#define BQ27227_H

#include "Arduino.h"
#include <Wire.h>
#include "bq27220_def.h"

#define DEFAULT_SCL  18
#define DEFAULT_SDA  8

enum CURR_MODE{
    CURR_RAW,
    CURR_INSTANT,
    CURR_STANDBY,
    CURR_CHARGING,
    CURR_AVERAGE,
};

enum VOLT_MODE{
    VOLT,
    VOLT_CHARGING,
    VOLT_RWA
};

typedef union BatteryStatus {
    struct __reg
    {
        // Low byte, Low bit first
        uint16_t DSG        : 1; /**< The device is in DISCHARGE */
        uint16_t SYSDWN     : 1; /**< System down bit indicating the system should shut down */
        uint16_t TDA        : 1; /**< Terminate Discharge Alarm */
        uint16_t BATTPRES   : 1; /**< Battery Present detected */
        uint16_t AUTH_GD    : 1; /**< Detect inserted battery */
        uint16_t OCVGD      : 1; /**< Good OCV measurement taken */
        uint16_t TCA        : 1; /**< Terminate Charge Alarm */
        uint16_t RSVD       : 1; /**< Reserved */
        // High byte, Low bit first
        uint16_t CHGING     : 1; /**< Charge inhibit */
        uint16_t FC         : 1; /**< Full-charged is detected */
        uint16_t OTD        : 1; /**< Overtemperature in discharge condition is detected */
        uint16_t OTC        : 1; /**< Overtemperature in charge condition is detected */
        uint16_t SLEEP      : 1; /**< Device is operating in SLEEP mode when set */
        uint16_t OCVFALL    : 1; /**< Status bit indicating that the OCV reading failed due to current */
        uint16_t OCVCOMP    : 1; /**< An OCV measurement update is complete */
        uint16_t FD         : 1; /**< Full-discharge is detected */
    } reg;
    uint16_t full;
}BQ27220BatteryStatus;

typedef enum {
    Bq27220OperationStatusSecSealed = 0b11,
    Bq27220OperationStatusSecUnsealed = 0b10,
    Bq27220OperationStatusSecFull = 0b01,
} Bq27220OperationStatusSec;

typedef union OperationStatus{
    struct __reg
    {
        // Low byte, Low bit first
        bool CALMD      : 1; /**< Calibration mode enabled */
        uint8_t SEC     : 2; /**< Current security access */
        bool EDV2       : 1; /**< EDV2 threshold exceeded */
        bool VDQ        : 1; /**< Indicates if Current discharge cycle is NOT qualified or qualified for an FCC updated */
        bool INITCOMP   : 1; /**< gauge initialization is complete */
        bool SMTH       : 1; /**< RemainingCapacity is scaled by smooth engine */
        bool BTPINT     : 1; /**< BTP threshold has been crossed */
        // High byte, Low bit first
        uint8_t RSVD1   : 2; /**< Reserved */
        bool CFGUPDATE  : 1; /**< Gauge is in CONFIG UPDATE mode */
        uint8_t RSVD0   : 5; /**< Reserved */
    } reg;
    uint16_t full;
} BQ27220OperationStatus;


typedef struct BQ27220DMData BQ27220DMData;

class BQ27220{
public:
    BQ27220() : addr{BQ27220_I2C_ADDRESS}, wire(&Wire), scl(DEFAULT_SCL), sda(DEFAULT_SDA)
    {}

    bool begin()
    {
        Wire.begin(DEFAULT_SDA, DEFAULT_SCL);
        return true;
    }

    uint16_t getTemp() {
        return ReadRegU16(BQ27220_COMMAND_TEMP);
    }

    uint16_t getBatterySt(void){
        return ReadRegU16(BQ27220_COMMAND_BATTERY_ST);
    }

    bool getIsCharging(void){
        uint16_t ret = ReadRegU16(BQ27220_COMMAND_BATTERY_ST);
        bat_st.full = ret;
        return !bat_st.reg.DSG;
    }

    uint16_t getRemainCap() {
        return ReadRegU16(BQ27220_COMMAND_REMAIN_CAPACITY);
    }

    uint16_t getChargePcnt(void)
    {
        return ReadRegU16(BQ27220_COMMAND_STATE_CHARGE);
    }

    uint16_t getAvgPower(void)
    {
        return ReadRegU16(CommandAveragePower);
    }

    uint16_t getFullChargeCap(void){
        return ReadRegU16(BQ27220_COMMAND_FCHG_CAPATICY);
    }

    uint16_t getVolt(VOLT_MODE type) {
        switch (type)
        {
        case VOLT:
            return ReadRegU16(BQ27220_COMMAND_VOLT);
            break;
        case VOLT_CHARGING:
            return ReadRegU16(BQ27220_COMMAND_CHARGING_VOLT);
            break;
        case VOLT_RWA:
            return ReadRegU16(BQ27220_COMMAND_RAW_VOLT);
            break;
        default:
            break;
        }
        return 0;
    }

    int16_t getCurr(CURR_MODE type) {
        switch (type)
        {
        case CURR_RAW:
            return (int16_t)ReadRegU16(BQ27220_COMMAND_RAW_CURR);
            break;
        case CURR_INSTANT:
            return (int16_t)ReadRegU16(BQ27220_COMMAND_CURR);
            break;
        case CURR_STANDBY:
            return (int16_t)ReadRegU16(BQ27220_COMMAND_STANDBY_CURR);
            break;
        case CURR_CHARGING:
            return (int16_t)ReadRegU16(BQ27220_COMMAND_CHARGING_CURR);
            break;
        case CURR_AVERAGE:
            return (int16_t)ReadRegU16(BQ27220_COMMAND_AVG_CURR);
            break;
        
        default:
            break;
        }
        return -1;
    }

    uint16_t ReadRegU16(uint16_t subAddress) {
        uint8_t data[2];
        i2cReadBytes(subAddress, data, 2);
        return ((uint16_t) data[1] << 8) | data[0];
    }

    uint16_t readCtrlWord(uint16_t fun) {
        uint8_t msb = (fun >> 8);
        uint8_t lsb = (fun & 0x00FF);
        uint8_t cmd[2] = { lsb, msb };
        uint8_t data[2] = {0};

        i2cWriteBytes((uint8_t)BQ27220_COMMAND_CONTROL, cmd, 2);

        if (i2cReadBytes((uint8_t) 0, data, 2)) {
            return ((uint16_t)data[1] << 8) | data[0];
        }
        return 0;
    }

    bool reset(void);
    bool init();

    // Sealed Access
    bool seal_access(void);
    bool unseal_access(void);
    bool full_access(void);

    // sub-commands
    uint16_t getDeviceNumber(void);

    bool getOperationStatus(BQ27220OperationStatus *oper_sta);


    bool controlSubCmd(uint16_t sub_cmd)
    {
        uint8_t msb = (sub_cmd >> 8);
        uint8_t lsb = (sub_cmd & 0x00FF);
        uint8_t buf[2] = { lsb, msb };
        i2cWriteBytes(CommandControl, buf, 2);
        return true;
    }

    bool i2cReadBytes(uint8_t subAddress, uint8_t * dest, uint8_t count) {
        Wire.beginTransmission(addr);
        Wire.write(subAddress);
        Wire.endTransmission(true);

        Wire.requestFrom(addr, count);
        for(int i = 0; i < count; i++) {
            dest[i] = Wire.read();
        }
        return true;
    }

    bool i2cWriteBytes(uint8_t subAddress, uint8_t * src, uint8_t count) {
        Wire.beginTransmission(addr);
        Wire.write(subAddress);
        for(int i = 0; i < count; i++) {
            Wire.write(src[i]);
        }
        Wire.endTransmission(true);
        return true;
    }

private:
    TwoWire *wire = NULL;
    uint8_t addr = 0;
    int scl = -1;
    int sda = -1;
    BQ27220BatteryStatus bat_st;
};

#endif

