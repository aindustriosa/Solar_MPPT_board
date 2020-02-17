#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void init_INA219(char mode){
    ina219.begin();
    ina219.setCalibration_32V_1A();
    // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
    //ina219.setCalibration_16V_400mA();
}

float get_LoadCurrent(){
    return ina219.getCurrent_mA();
}

float get_LoadVolts(){
    float shuntvoltage = 0;
    float busvoltage = 0;
    shuntvoltage = ina219.getShuntVoltage_mV();
    busvoltage = ina219.getBusVoltage_V();
    return busvoltage + (shuntvoltage / 1000);
}
