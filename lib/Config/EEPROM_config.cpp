#include <EEPROM.h>
#include <EEPROM_config.h>

#define EEPROM_SIGNATURE 0x42  // A unique signature to indicate valid EEPROM data

extern uint8_t motorMaxSpeed;
extern uint8_t motorWeight;
extern float motorTurnFactor;

const int _motorMaxSpeedAddr = 1;     // Address 1 for motor max speed (uint8_t)
const int _motorWeightAddr = 2;       // Address 2 for motor weight (uint8_t)
const int _motorTurnFactorAddr = 4;   // Address 4 for motorTurnFactor (float requires 4 bytes)

EEPROMConfig::EEPROMConfig() {
    EEPROM.begin(512);  // Initialize EEPROM with 512 bytes
}

EEPROMConfig::~EEPROMConfig() {}

void EEPROMConfig::loadSettings() {
    // Check if EEPROM contains the correct signature
    uint8_t signature = EEPROM.read(0);  // Signature stored at address 0

    if (signature == EEPROM_SIGNATURE) {
        // Load values from EEPROM
        EEPROM.get(_motorMaxSpeedAddr, motorMaxSpeed);
        EEPROM.get(_motorWeightAddr, motorWeight);
        EEPROM.get(_motorTurnFactorAddr, motorTurnFactor);

        // Debugging: Print loaded values from EEPROM
        Serial.printf("Loaded from EEPROM - Max Speed: %d, Weight: %d, Turn Factor: %.2f\n", motorMaxSpeed, motorWeight, motorTurnFactor);
    } else {
        // EEPROM is uninitialized, don't set any default values here
        Serial.println("EEPROM is uninitialized. Waiting for user to set values.");
    }
}

void EEPROMConfig::saveMotorMaxSpeed(uint8_t newMotorMaxSpeed) {
    EEPROM.put(_motorMaxSpeedAddr, newMotorMaxSpeed);
    EEPROM.commit();  // Ensure the changes are saved

    // Debugging: Verify by reading the value back and printing
    uint8_t readBack;
    EEPROM.get(_motorMaxSpeedAddr, readBack);
    Serial.printf("Motor Max Speed Saved: %d, Read Back: %d\n", newMotorMaxSpeed, readBack);
}

void EEPROMConfig::saveMotorWeight(uint8_t newMotorWeight) {
    EEPROM.put(_motorWeightAddr, newMotorWeight);
    EEPROM.commit();  // Ensure the changes are saved

    // Debugging: Verify by reading the value back and printing
    uint8_t readBack;
    EEPROM.get(_motorWeightAddr, readBack);
    Serial.printf("Motor Weight Saved: %d, Read Back: %d\n", newMotorWeight, readBack);
}

void EEPROMConfig::saveMotorTurnFactor(float newMotorTurnFactor) {
    EEPROM.put(_motorTurnFactorAddr, newMotorTurnFactor);  // Save float value
    EEPROM.commit();  // Ensure the changes are saved

    // Debugging: Verify by reading the value back and printing
    float readBack;
    EEPROM.get(_motorTurnFactorAddr, readBack);
    Serial.printf("Motor Turn Factor Saved: %.2f, Read Back: %.2f\n", newMotorTurnFactor, readBack);
}

// Function to save the signature after all values are set for the first time
void EEPROMConfig::saveSignature() {
    EEPROM.write(0, EEPROM_SIGNATURE);  // Write the signature after settings are saved
    EEPROM.commit();
    Serial.println("Signature saved to EEPROM.");
}