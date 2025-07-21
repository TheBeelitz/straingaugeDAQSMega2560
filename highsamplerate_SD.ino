/**
   Full-scale 16-channel strain gauge measurement using INA333 + ADS1115 with Arduino Mega

   Components:
     ➤ 15 strain gauges (quarter-bridge)
     ➤ 15 INA333 amplifiers (CJMCU-333)
     ➤ 4 ADS1115 (4 channels each, I²C)
     ➤ Arduino Mega 2560

   Features:
     - 16-channel auto-zero calibration
     - Real-time Serial output (Binary format for higher speed)
     - SD card logging for high-frequency data capture
     - Timer interrupt-driven sampling (Optimized for 860 SPS)
     - Watchdog timer protection

   Wiring:
     ➤ I²C: SDA → Mega pin 20, SCL → Mega pin 21
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <SD.h>
#include <TimerOne.h>   // Timer interrupt for precise sampling
#include <avr/wdt.h>    // Watchdog timer for system stability

// === CONFIGURATION ===
const int numChannels = 16;
const int chipSelect = 53;  // SD card CS pin

Adafruit_ADS1115 adsModules[4];
uint8_t i2cAddresses[4] = {0x48, 0x49, 0x4A, 0x4B};

float zeroOffsets[numChannels];   // Auto-zero (tare)
float gainFactors[numChannels];   // Scale (if calibrated)

File dataFile;  // SD card file handler

// === FUNCTION DECLARATIONS ===
void sampleSensors();
void calibrateZeroAll();

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    wdt_enable(WDTO_250MS);  // Watchdog timer: Resets if system freezes
    
    Serial.println("START");  // For sync with external logging software

    // Initialize SD card
    if (!SD.begin(chipSelect)) {
        Serial.println("SD card not found! Logging disabled.");
    } else {
        dataFile = SD.open("strain_log.bin", FILE_WRITE);
    }

    // Initialize ADS1115 modules
    for (int i = 0; i < 4; i++) {
        adsModules[i] = Adafruit_ADS1115();
        if (!adsModules[i].begin(i2cAddresses[i])) {
            Serial.print("ADS1115 at 0x");
            Serial.print(i2cAddresses[i], HEX);
            Serial.println(" not found!");
            while (1);
        }
        adsModules[i].setGain(GAIN_ONE);  // ±4.096V range for strain gauge readings
        adsModules[i].setDataRate(RATE_ADS1115_860SPS);  // Max sampling rate
    }

    for (int i = 0; i < numChannels; i++) {
        gainFactors[i] = 797.6;  // Generalized scaling factor for all readings
        zeroOffsets[i] = 0.0;
    }

    calibrateZeroAll();

    Timer1.initialize(1160);  // Interrupt every ~1.16ms for 860 SPS sampling
    Timer1.attachInterrupt(sampleSensors);

    Serial.println("System Ready.");
}

// === INTERRUPT-DRIVEN SENSOR SAMPLING ===
void sampleSensors() {
    static float readings[numChannels];
    int sensorIndex = 0;

    for (int module = 0; module < 4; module++) {
        for (int channel = 0; channel < 4; channel++) {
            int16_t raw = adsModules[module].readADC_SingleEnded(channel);
            float voltage = adsModules[module].computeVolts(raw);
            readings[sensorIndex] = (voltage - zeroOffsets[sensorIndex]);
            sensorIndex++;
        }
    }

    // **Binary Data Transmission for Higher Efficiency**
    Serial.write((uint8_t*)&readings, sizeof(readings));

    // **SD Card Logging (Only if SD is available)**
    if (dataFile) {
        dataFile.write((uint8_t*)&readings, sizeof(readings));
        dataFile.flush();
    }

    wdt_reset();  // Reset watchdog to prevent system freeze
}

// === ZERO CALIBRATION ===
void calibrateZeroAll() {
    const int samples = 50;
    Serial.println("Calibrating zero offsets...");
    
    int sensorIndex = 0;
    for (int module = 0; module < 4; module++) {
        for (int channel = 0; channel < 4; channel++) {
            long sum = 0;
            for (int i = 0; i < samples; i++) {
                sum += adsModules[module].readADC_SingleEnded(channel);
            }
            float avgVoltage = adsModules[module].computeVolts(sum / samples);
            zeroOffsets[sensorIndex] = avgVoltage;
            Serial.println(zeroOffsets[sensorIndex], 6);
            sensorIndex++;
        }
    }
    
    Serial.println("Calibration complete.");
}
