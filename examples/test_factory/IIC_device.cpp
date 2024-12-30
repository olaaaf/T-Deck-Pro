#include "test_factory.h"
#include "SensorLTR553.hpp"
#include "SensorBHI260AP.hpp"
#include <XPowersLib.h>

#if 1 // Light sensor 
SensorLTR553 als;

bool LTR_553ALS_init(int i2c_addr)
{
    pinMode(BOARD_ALS_INT, INPUT);

    if(!als.begin(Wire, i2c_addr, BOARD_ALS_SDA, BOARD_ALS_SCL)){
        log_e("LTR_553ALS not found, check wiring & pullups!");
        return false;
    }
    Serial.println("Init LTR553 Sensor success!");

    // Set the ambient light high and low thresholds.
    // If the value exceeds or falls below the set value, an interrupt will be triggered.
    als.setLightSensorThreshold(10, 200);

    // Set the high and low thresholds of the proximity sensor.
    // If the value exceeds or falls below the set value, an interrupt will be triggered.
    // als.setProximityThreshold(10, 30);

    // Controls the Light Sensor N number of times the measurement data is outside the range
    // defined by the upper and lower threshold limits before asserting the interrupt.
    als.setLightSensorPersists(1);

    // Controls the Proximity  N number of times the measurement data is outside the range
    // defined by the upper and lower threshold limits before asserting the interrupt.
    als.setProximityPersists(1);

    /*
    *  ALS_IRQ_ACTIVE_LOW, // INT pin is considered active when it is a logic 0 (default)
    *  ALS_IRQ_ACTIVE_HIGH // INT pin is considered active when it is a logic 1
    * * */
    als.setIRQLevel(SensorLTR553::ALS_IRQ_ACTIVE_LOW);

    /*
    *  ALS_IRQ_ONLY_PS,    // Only PS measurement can trigger interrupt
    *  ALS_IRQ_ONLY_ALS,   // Only ALS measurement can trigger interrupt
    *  ALS_IRQ_BOTH,       // Both ALS and PS measurement can trigger interrupt
    * * * */
    als.enableIRQ(SensorLTR553::ALS_IRQ_BOTH);

    /*
    *   ALS_GAIN_1X  ,   -> 1 lux to 64k lux (default)
    *   ALS_GAIN_2X  ,   -> 0.5 lux to 32k lux
    *   ALS_GAIN_4X  ,   -> 0.25 lux to 16k lux
    *   ALS_GAIN_8X  ,   -> 0.125 lux to 8k lux
    *   ALS_GAIN_48X ,   -> 0.02 lux to 1.3k lux
    *   ALS_GAIN_96X ,   -> 0.01 lux to 600 lux
    * */
    als.setLightSensorGain(SensorLTR553::ALS_GAIN_1X);

    /*
    *   PS_LED_PLUSE_30KHZ,
    *   PS_LED_PLUSE_40KHZ,
    *   PS_LED_PLUSE_50KHZ,
    *   PS_LED_PLUSE_60KHZ,
    *   PS_LED_PLUSE_70KHZ,
    *   PS_LED_PLUSE_80KHZ,
    *   PS_LED_PLUSE_90KHZ,
    *   PS_LED_PLUSE_100KHZ,
    * * * * * * * * * * */
    als.setPsLedPulsePeriod(SensorLTR553::PS_LED_PLUSE_100KHZ);


    /*
    *   PS_LED_DUTY_25,
    *   PS_LED_DUTY_50,
    *   PS_LED_DUTY_75,
    *   PS_LED_DUTY_100,
    * * * */
    als.setPsLedDutyCycle(SensorLTR553::PS_LED_DUTY_100);

    /*
    *   PS_LED_CUR_5MA,
    *   PS_LED_CUR_10MA,
    *   PS_LED_CUR_20MA,
    *   PS_LED_CUR_50MA,
    *   PS_LED_CUR_100MA,
    * * * * * * * */
    als.setPsLedCurrent(SensorLTR553::PS_LED_CUR_100MA);

    /*
    *   PS_MEAS_RATE_50MS,
    *   PS_MEAS_RATE_70MS,
    *   PS_MEAS_RATE_100MS,
    *   PS_MEAS_RATE_200MS,
    *   PS_MEAS_RATE_500MS,
    *   PS_MEAS_RATE_1000MS,
    *   PS_MEAS_RATE_2000MS,
    *   PS_MEAS_RATE_10MS
    * * * * * * * */
    als.setProximityRate(SensorLTR553::PS_MEAS_RATE_50MS);

    // Number of pulses
    als.setPsLedPulses(1);

    // Enable ambient light sensor
    als.enableLightSensor();

    // Enable proximity sensor
    als.enableProximity();

    // Enable proximity sensor saturation indication
    als.enablePsIndicator();
    return true;
}

uint16_t LTR_553ALS_get_channel(int ch) // ch 0~1
{
    return als.getLightSensor(ch);
}

uint16_t LTR_553ALS_get_ps(void)
{
    bool saturated = false;
    return als.getProximity(&saturated);
}

#endif

#if 1 // Gyroscope
SensorBHI260AP bhy;
struct bhy2_data_xyz accel_data;
struct bhy2_data_xyz gyro_data;
struct bhy2_data_xyz magn_data;

float accel_factor;
float gyro_factor;
float magn_factor;

void accel_process_callback(uint8_t sensor_id, uint8_t *data_ptr, uint32_t len, uint64_t *timestamp)
{
    accel_factor = get_sensor_default_scaling(sensor_id);
    bhy2_parse_xyz(data_ptr, &accel_data);
}

void gyro_process_callback(uint8_t sensor_id, uint8_t *data_ptr, uint32_t len, uint64_t *timestamp)
{
    gyro_factor = get_sensor_default_scaling(sensor_id);
    bhy2_parse_xyz(data_ptr, &gyro_data);
}

void magn_process_callback(uint8_t sensor_id, uint8_t *data_ptr, uint32_t len, uint64_t *timestamp)
{
    magn_factor = get_sensor_default_scaling(sensor_id);
    bhy2_parse_xyz(data_ptr, &magn_data);
}

bool Gyroscope_init(int i2c_addr)
{
    bhy.setPins(BOARD_GYROSCOPDE_RST, BOARD_GYROSCOPDE_INT);

    // Using I2C interface
    // BHI260AP_SLAVE_ADDRESS_L = 0x28
    // BHI260AP_SLAVE_ADDRESS_H = 0x29
    if (!bhy.init(Wire, BOARD_GYROSCOPDE_SCL, BOARD_GYROSCOPDE_SDA, i2c_addr)) {
        log_e("Failed to init BHI260AP -");
        Serial.println(bhy.getError());
        return true;
    }

    // bhy.printSensors(Serial);

    float sample_rate = 100.0;      /* Read out hintr_ctrl measured at 100Hz */
    uint32_t report_latency_ms = 0; /* Report immediately */

    // Enable acceleration
    bhy.configure(SENSOR_ID_ACC_PASS, sample_rate, report_latency_ms);
    // Enable gyroscope
    bhy.configure(SENSOR_ID_GYRO_PASS, sample_rate, report_latency_ms);
    // Enable Magnetometer
    bhy.configure(SENSOR_ID_MAG_PASS, sample_rate, report_latency_ms);


    // Set the acceleration sensor result callback function
    bhy.onResultEvent(SENSOR_ID_ACC_PASS, accel_process_callback);

    // Set the gyroscope sensor result callback function
    bhy.onResultEvent(SENSOR_ID_GYRO_PASS, gyro_process_callback);

    // Set the Magnetometer sensor result callback function
    bhy.onResultEvent(SENSOR_ID_MAG_PASS, magn_process_callback);
    
    return true;
}

/**
 * int val_type 
 *      1 --- acceleration
 *      2 --- gyroscope
 *      3 --- Magnetometer
*/
void Gyroscope_get_val(int val_type, float *x, float *y, float *z)
{
    bhy.update();

    switch (val_type) {
        case 1:
            *x = accel_data.x * accel_factor;
            *y = accel_data.y * accel_factor;
            *z = accel_data.z * accel_factor;
         break;
        case 2 :
            *x = gyro_data.x * gyro_factor;
            *y = gyro_data.y * gyro_factor;
            *z = gyro_data.z * gyro_factor;
         break;
        case 3:
            *x = magn_data.x * magn_factor;
            *y = magn_data.y * magn_factor;
            *z = magn_data.z * magn_factor;
         break;
        default:
            *x = 0;
            *y = 0;
            *z = 0;
            break;
    }
}
#endif

#if 1 // PMU SY6970
PowersSY6970 PMU;
uint32_t cycleInterval = 1000;

bool PMU_init(int addr)
{
    bool result =  PMU.init(Wire, BOARD_PMU_SY6970_SDA, BOARD_PMU_SY6970_SCL, addr);

    if (result == false) {
        Serial.println("PMU is not online...");
    }

    // Set the charging target voltage, Range:3840 ~ 4608mV ,step:16 mV
    PMU.setChargeTargetVoltage(3856);

    // Set the precharge current , Range: 64mA ~ 1024mA ,step:64mA
    PMU.setPrechargeCurr(64);

    // Set the charging current , Range:0~5056mA ,step:64mA
    PMU.setChargerConstantCurr(320);

    // To obtain voltage data, the ADC must be enabled first
    PMU.enableADCMeasure();

    return result;
}

void PMU_loop(void)
{
    // SY6970 When VBUS is input, the battery voltage detection will not take effect
    if (millis() > cycleInterval) {

        Serial.println("Sats        VBUS    VBAT   SYS    VbusStatus      String   ChargeStatus     String      TargetVoltage       ChargeCurrent       Precharge       NTCStatus           String");
        Serial.println("            (mV)    (mV)   (mV)   (HEX)                         (HEX)                    (mV)                 (mA)                   (mA)           (HEX)           ");
        Serial.println("--------------------------------------------------------------------------------------------------------------------------------");
        Serial.print(PMU.isVbusIn() ? "Connected" : "Disconnect"); Serial.print("\t");
        Serial.print(PMU.getVbusVoltage()); Serial.print("\t");
        Serial.print(PMU.getBattVoltage()); Serial.print("\t");
        Serial.print(PMU.getSystemVoltage()); Serial.print("\t");
        Serial.print("0x");
        Serial.print(PMU.getBusStatus(), HEX); Serial.print("\t");
        Serial.print(PMU.getBusStatusString()); Serial.print("\t");
        Serial.print("0x");
        Serial.print(PMU.chargeStatus(), HEX); Serial.print("\t");
        Serial.print(PMU.getChargeStatusString()); Serial.print("\t");

        Serial.print(PMU.getChargeTargetVoltage()); Serial.print("\t");
        Serial.print(PMU.getChargerConstantCurr()); Serial.print("\t");
        Serial.print(PMU.getPrechargeCurr()); Serial.print("\t");
        Serial.print(PMU.getNTCStatus()); Serial.print("\t");
        Serial.print(PMU.getNTCStatusString()); Serial.print("\t");

        Serial.println();
        Serial.println();
        cycleInterval = millis() + 1000;
    }
}

#endif
