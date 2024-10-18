#include <EEPROM_config.h>

EEPROMConfig::EEPROMConfig() {
    EEPROM.begin(512);
}

EEPROMConfig::~EEPROMConfig() {
    //
}


void EEPROMConfig::loadSettings(uint8_t &motorMaxSpeed, uint8_t &motorWeight, float &motorTurnFactor) {
    // Read values from EEPROM
    motorMaxSpeed = EEPROM.read(_motorMaxSpeedAddr);
    motorWeight = EEPROM.read(_motorWeightAddr);
    EEPROM.get(_motorTurnFactorAddr, motorTurnFactor);

    // Define default values for each variable
    uint8_t defaultMaxSpeed = 60;  // Default motor max speed
    uint8_t defaultMotorWeight = 45;  // Default motor weight
    float defaultTurnFactor = 0.15;  // Default turn factor

    // Check if EEPROM values are invalid (e.g., 0 or out of reasonable range)
    bool eepromChanged = false;

    if (motorMaxSpeed == 0 || motorMaxSpeed > 255) {
        motorMaxSpeed = defaultMaxSpeed;  // Set default if invalid
        EEPROM.write(_motorMaxSpeedAddr, motorMaxSpeed);  // Save default to EEPROM
        eepromChanged = true;
    }

    if (motorWeight == 0 || motorWeight > 255) {
        motorWeight = defaultMotorWeight;  // Set default if invalid
        EEPROM.write(_motorWeightAddr, motorWeight);  // Save default to EEPROM
        eepromChanged = true;
    }

    if (motorTurnFactor == 0.0f || motorTurnFactor > 1.0f) {
        motorTurnFactor = defaultTurnFactor;  // Set default if invalid
        EEPROM.put(_motorTurnFactorAddr, motorTurnFactor);  // Save default to EEPROM
        eepromChanged = true;
    }

    // Commit changes to EEPROM only if any defaults were written
    if (eepromChanged) {
        EEPROM.commit();
        Serial.println("Default values written to EEPROM.");
    }

    // Output the loaded or default values to Serial (optional)
    Serial.printf("Max Speed: %d, Weight: %d, Turn Factor: %.2f\n", motorMaxSpeed, motorWeight, motorTurnFactor);
}

void EEPROMConfig::saveMotorMaxSpeed(uint8_t motorMaxSpeed) {
    if (EEPROM.read(_motorMaxSpeedAddr) != motorMaxSpeed) {  // Only write if value changed
        EEPROM.write(_motorMaxSpeedAddr, motorMaxSpeed);
        EEPROM.commit();  // Commit changes
    }
}

void EEPROMConfig::saveMotorWeight(uint8_t motorWeight) {
    if (EEPROM.read(_motorWeightAddr) != motorWeight) {  // Only write if value changed
        EEPROM.write(_motorWeightAddr, motorWeight);
        EEPROM.commit();  // Commit changes
    }
}

void EEPROMConfig::saveMotorTurnFactor(float motorTurnFactor) {
    float storedValue;
    EEPROM.get(_motorTurnFactorAddr, storedValue);  // Read stored value
    if (storedValue != motorTurnFactor) {  // Only write if value changed
        EEPROM.put(_motorTurnFactorAddr, motorTurnFactor);  // Save float value
        EEPROM.commit();  // Commit changes
    }
}