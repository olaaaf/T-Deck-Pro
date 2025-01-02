#include "bq27220.h"
#include "bq27220_data_memory.h"

#define BQ27220_ID (0x0220u)

/** Timeout for common operations. */
#define BQ27220_TIMEOUT_COMMON_US (2000000u)


/** Timeout cycle interval  */
#define BQ27220_TIMEOUT_CYCLE_INTERVAL_US (1000u)

/** Timeout cycles count helper */
#define BQ27220_TIMEOUT(timeout_us) ((timeout_us) / (BQ27220_TIMEOUT_CYCLE_INTERVAL_US))

bool BQ27220::init()
{
    bool result = false;
    do{
        uint16_t data = getDeviceNumber();
        if(data != BQ27220_ID) {
            Serial.printf("Invalid Device Number %04x != 0x0220\n", data);
            break;
        }
        result = true;
    } while(0);
    return result;
}

bool BQ27220::reset(void)
{
    bool result = false;
    BQ27220OperationStatus operat = {0};
    do{
        controlSubCmd(Control_RESET);
        delay(10);

        uint32_t timeout = BQ27220_TIMEOUT(BQ27220_TIMEOUT_COMMON_US);
        while (--timeout)
        {
            if(!getOperationStatus(&operat)){
                Serial.printf("Failed to get operation status, retries left %lu\n", timeout);
            }else if(operat.reg.INITCOMP == true){
                break;
            }
            delay(2);
        }
        if(timeout == 0) {
            Serial.println("INITCOMP timeout after reset");
            break;
        }
        Serial.printf("Cycles left: %lu\n", timeout);
        result = true;
    } while(0);
    return result;
}

// Sealed Access
bool BQ27220::seal_access(void) 
{
    bool result = false;
    BQ27220OperationStatus operat = {0};
    do{
        getOperationStatus(&operat);
        if(operat.reg.SEC == Bq27220OperationStatusSecSealed)
        {
            result = true;
            break;
        }

        controlSubCmd(Control_SEALED);
        delay(10);

        getOperationStatus(&operat);
        if(operat.reg.SEC != Bq27220OperationStatusSecSealed)
        {
            Serial.printf("Seal failed %u\n", operat.reg.SEC);
            break;
        }
        result = true;
    } while(0);

    return result;
}

bool BQ27220::unseal_access(void) 
{
    bool result = false;
    BQ27220OperationStatus operat = {0};
    do{
        getOperationStatus(&operat);
        if(operat.reg.SEC != Bq27220OperationStatusSecSealed)
        {
            result = true;
            break;
        }

        controlSubCmd(UnsealKey1);
        delay(10);
        controlSubCmd(UnsealKey2);
        delay(10);

        getOperationStatus(&operat);
        if(operat.reg.SEC != Bq27220OperationStatusSecUnsealed)
        {
            Serial.printf("Unseal failed %u\n", operat.reg.SEC);
            break;
        }
        result = true;
    } while (0);

    return result;
}

bool BQ27220::full_access(void) 
{
    bool result = false;
    BQ27220OperationStatus operat = {0};

    do{
        uint32_t timeout = BQ27220_TIMEOUT(BQ27220_TIMEOUT_COMMON_US);
        while (--timeout)
        {
            if(!getOperationStatus(&operat)){
                Serial.printf("Failed to get operation status, retries left %lu\n", timeout);
            }else {
                break;
            }
        }
        if(timeout == 0) {
            Serial.println("Failed to get operation status");
            break;
        }
        Serial.printf("Cycles left: %lu\n", timeout);

        // Already full access
        if(operat.reg.SEC == Bq27220OperationStatusSecFull){
            result = true;
            break;
        }
        // Must be unsealed to get full access
        if(operat.reg.SEC != Bq27220OperationStatusSecUnsealed){
            Serial.println("Not in unsealed state");
            break;
        }

        controlSubCmd(FullAccessKey);
        delay(10);
        controlSubCmd(FullAccessKey);
        delay(10);

        if(!getOperationStatus(&operat)){
            Serial.println("Status query failed");
            break;
        }
        if(operat.reg.SEC != Bq27220OperationStatusSecFull){
            Serial.printf("Full access failed %u\n", operat.reg.SEC);
            break;
        }
        result = true;
    } while (0);
    return result;
}

uint16_t BQ27220::getDeviceNumber(void)
{
    uint16_t devid = 0;
    // Request device number(chip PN)
    controlSubCmd(Control_DEVICE_NUMBER);
    // Enterprise wait(MAC read fails if less than 500us)
    // bqstudio uses ~15ms
    delay(15);
    // Read id data from MAC scratch space
    i2cReadBytes(CommandMACData, (uint8_t *)&devid, 2);

    // Serial.printf("device number:0x%x\n", devid);
    return devid;
}

bool BQ27220::getOperationStatus(BQ27220OperationStatus *oper_sta)
{
    bool result = false;
    uint16_t data = ReadRegU16(CommandOperationStatus);
    if(data != 0)
    {
        (*oper_sta).full = data;
        result = true;
    }
    return result;
}
