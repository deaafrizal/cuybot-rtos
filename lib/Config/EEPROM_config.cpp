#include <EEPROM_config.h>

uint8_t EEPROMConfig::motorMaxSpeed = 75;
uint8_t EEPROMConfig::motorWeight = 50;
float EEPROMConfig::motorTurnFactor = 0.15;

EEPROMConfig::EEPROMConfig() {}

EEPROMConfig::~EEPROMConfig() {}

void EEPROMConfig::loadSettings() {
    if (EEPROM.read(_MAGIC_MEM_ADDR) != _MAGIC_MEM_NUM) {
        EEPROM.write(1, motorMaxSpeed);
        EEPROM.write(2, motorWeight);
        EEPROM.write(_MAGIC_MEM_ADDR, _MAGIC_MEM_NUM);
        EEPROM.commit();
        setMemFloat(10, motorTurnFactor);
        Serial.println("EEPROM set!");
    }else {
        motorMaxSpeed = getMemInt(1);
        motorWeight = getMemInt(2);
        motorTurnFactor = getMemFloat(10);
        Serial.println("EEPROM get latest data!");
    }
    Serial.println("EEPROM setup success.");
}

void EEPROMConfig::setMemInt(int addr, uint8_t newMemInt) {
    EEPROM.write(addr, newMemInt);
    EEPROM.commit();
}

void EEPROMConfig::setMemFloat(int addr, float newMemFloat) {
    byte* bytePointer = (byte*)&newMemFloat;

    for (int i = 0; i < sizeof(float); i++) {
        EEPROM.write(addr + i, bytePointer[i]);
    }

    EEPROM.commit();
    Serial.print("saved to memFloat: ");
    Serial.print(newMemFloat);
    Serial.print(" addr: ");
    Serial.println(addr);
}

uint8_t EEPROMConfig::getMemInt(int addr) {
    return EEPROM.read(addr);
}

float EEPROMConfig::getMemFloat(int addr) {
    float value = 0.0;
    byte* bytePointer = (byte*)&value;

    for (int i = 0; i < sizeof(float); i++) {
        bytePointer[i] = EEPROM.read(addr + i);
    }

    return value;
}