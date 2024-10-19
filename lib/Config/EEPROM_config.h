#ifndef EEPROM_CONFIG_H
#define EEPROM_CONFIG_H

#include <EEPROM.h>

class EEPROMConfig {
public:
    EEPROMConfig();
    ~EEPROMConfig();
    void loadSettings();
    void saveMotorMaxSpeed(uint8_t newMotorMaxSpeed);
    void saveMotorWeight(uint8_t newMotorWeight);
    void saveMotorTurnFactor(float newMotorTurnFactor);
    void saveSignature();

private:
    const int _motorMaxSpeedAddr = 1;
    const int _motorWeightAddr = 2;
    const int _motorTurnFactorAddr = 4;
};

#endif