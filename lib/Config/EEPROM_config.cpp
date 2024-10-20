#include <EEPROM.h>
#include <EEPROM_config.h>

extern uint8_t motorMaxSpeed;
extern uint8_t motorWeight;
extern float motorTurnFactor;

EEPROMConfig::EEPROMConfig() {
    EEPROM.begin(_MEM_SIZE);
}

EEPROMConfig::~EEPROMConfig() {}

void EEPROMConfig::loadSettings() {
    if (EEPROM.read(_MAGIC_MEM_ADDR) != _MAGIC_MEM_NUM) {
        setMemInt(1, motorMaxSpeed);
        setMemInt(2, motorWeight);
        setMemFloat(10, motorTurnFactor);
        setMemInt(_MAGIC_MEM_ADDR, _MAGIC_MEM_NUM);
        Serial.println("added default EEPROM.");
    }else {
        Serial.println("default value loaded!");
    }
    Serial.println("EEPROM setup success!");
}

void EEPROMConfig::setMemInt(int addr, uint8_t newMemInt) {
    EEPROM.write(addr, newMemInt);
    EEPROM.commit();
    Serial.print("saved to memInt: ");
    Serial.print(newMemInt);
    Serial.print(" addr: ");
    Serial.println(addr);
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
    _memInt = EEPROM.read(addr);
    return _memInt;
}

float EEPROMConfig::getMemFloat(int addr) {
    float value = 0.0;
    byte* bytePointer = (byte*)&value;

    for (int i = 0; i < sizeof(float); i++) {
        bytePointer[i] = EEPROM.read(addr + i);
    }

    return value;
}