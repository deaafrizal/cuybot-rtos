#ifndef EEPROM_CONFIG_H
#define EEPROM_CONFIG_H

#include <EEPROM.h>

class EEPROMConfig {
public:
    EEPROMConfig();
    ~EEPROMConfig();
    void loadSettings();
    void setMemInt(int addr, uint8_t newMemInt);
    void setMemFloat(int addr, float newMemFloat);
    uint8_t getMemInt(int addr);
    float getMemFloat(int addr);

private:
    uint8_t _memInt;
    float _memFloat;
    const uint8_t _MEM_SIZE = 128;
    const uint8_t _MAGIC_MEM_NUM = 42;
    const int _MAGIC_MEM_ADDR = 0;

};

#endif