#ifndef EEPROMCONFIG_H
#define EEPROMCONFIG_H

#include <EEPROM.h>

class EEPROMConfig {
public:
    static const uint8_t _MAGIC_MEM_NUM = 0xA5;
    static const uint8_t _MAGIC_MEM_ADDR = 0;

    EEPROMConfig();
    ~EEPROMConfig();

    static void loadSettings();
    static void setMemFloat(int addr, float newMemFloat);
    static void setMemInt(int addr, uint8_t newMemInt);
    static uint8_t getMemInt(int addr);
    static float getMemFloat(int addr);

    static uint8_t motorMaxSpeed;
    static uint8_t motorWeight;
    static float motorTurnFactor;
};

#endif 