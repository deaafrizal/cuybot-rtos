#ifndef EEPROM_CONFIG_H
#define EEPROM_CONFIG_H

#include <EEPROM.h>

class EEPROMConfig
{
public:
    EEPROMConfig();
    ~EEPROMConfig();
    
    // Methods to load and save settings
    void loadSettings(uint8_t &motorMaxSpeed, uint8_t &motorWeight, float &motorTurnFactor);
    void saveMotorMaxSpeed(uint8_t motorMaxSpeed);
    void saveMotorWeight(uint8_t motorWeight);
    void saveMotorTurnFactor(float motorTurnFactor);

private:
    const uint8_t _motorMaxSpeedAddr = 10;
    const uint8_t _motorWeightAddr = 12;
    const uint8_t _motorTurnFactorAddr = 14;
};

#endif