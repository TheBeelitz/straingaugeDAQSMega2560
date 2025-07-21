/**
   Full-scale 16-channel strain gauge measurement using INA333 + ADS1115 with Arduino Mega

   Components:
     ➤ 15 strain gauges (quarter-bridge)
     ➤ 15 INA333 amplifiers (CJMCU-333)
     ➤ 4 ADS1115 (4 channels each, I²C)
     ➤ Arduino Mega 2560

   Features:
     - 16-channel auto-zero calibration
     - Real-time Serial output (Serial Plotter compatible)
     - SD card logging temporarily disabled for testing

   Wiring:
     ➤ I²C: SDA → Mega pin 20, SCL → Mega pin 21
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// === CONFIGURATION ===
const int numChannels = 16;
const int chipSelect = 53;  // SD card CS pin (not used here)

Adafruit_ADS1115 adsModules[4];
uint8_t i2cAddresses[4] = {0x48, 0x49, 0x4A, 0x4B};

float zeroOffsets[numChannels];   // Auto-zero (tare)
float gainFactors[numChannels];   // Scale (if calibrated)

// === FUNCTION DECLARATIONS ===
void calibrateZeroAll();

void setup() {
  Serial.begin(115200); // setting serial baud rate for highest possible data read
  Wire.begin();

  Serial.println("START");  // For Python to sync on

  // Initialize ADS1115 modules
  for (int i = 0; i < 4; i++) {
    adsModules[i] = Adafruit_ADS1115();
    if (!adsModules[i].begin(i2cAddresses[i])) {
      Serial.print("ADS1115 at 0x");
      Serial.print(i2cAddresses[i], HEX);
      Serial.println(" not found!");
      while (1);
    }
    adsModules[i].setGain(GAIN_ONE);  //TWO: ±2.048 V range, ONE: ±4.096 V range, FOUR: ±1.024 V range

    adsModules[i].setDataRate(RATE_ADS1115_128SPS); // samples per second setting: 250SPS, 128SPS, 64SPS 
  }

  for (int i = 0; i < numChannels; i++) {
    gainFactors[i] = 797.6;     //  generalized for all readings with: INA with G = 501, 120Ohm Quarter Bridge, 5V Excitation, strain gauge k factor 2.0
    zeroOffsets[i] = 0.0;
  }

  calibrateZeroAll();

  // Optional: print header
  Serial.print("Time(ms)");
  for (int i = 0; i < numChannels; i++) {
    Serial.print(",CH");
    if (i < 9) Serial.print("0");
    Serial.print(i + 1);
  }
  Serial.println();
}

void loop() {
  static unsigned long lastSample = 0;
  unsigned long now = millis();

  // on-blocking timing control pattern for 
  if (now - lastSample >= 125) {  //128 SPS = 1 sample every ~7.8 ms; 16 readings × 7.8 ms ≈ 125 ms
    lastSample = now;

    float readings[numChannels];
    int sensorIndex = 0;

    for (int module = 0; module < 4; module++) {
      for (int channel = 0; channel < 4; channel++) {
        int16_t raw = adsModules[module].readADC_SingleEnded(channel);
        float voltage = adsModules[module].computeVolts(raw);
        readings[sensorIndex] = (voltage - zeroOffsets[sensorIndex]);// * gainFactors[sensorIndex];
        sensorIndex++;
      }
    }

    // === Output  CSV row ===
    Serial.print(now);  // Timestamp in ms
    for (int i = 0; i < numChannels; i++) {
      Serial.print(",");
      Serial.print(readings[i], 6);
    }
    Serial.println();
  }
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
      Serial.println(zeroOffsets[sensorIndex],6);
      sensorIndex++;
    }
    
  }
  
  Serial.println("Calibration complete.");
}
