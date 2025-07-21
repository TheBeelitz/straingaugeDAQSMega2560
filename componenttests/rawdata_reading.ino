/**
   Full-scale 16-channel strain gauge measurement using INA333 + ADS1115 with Arduino Mega

   Components:
     ➤ 15 strain gauges (quarter-bridge)
     ➤ 15 INA333 amplifiers (CJMCU-333)
     ➤ 4 ADS1115 (4 channels each, I²C)
     ➤ Arduino Mega 2560

   Features:
     - 16-channel raw data aquisition 
     - Serial output 
    

   Wiring:
     ➤ I²C: SDA → Mega pin 20, SCL → Mega pin 21
*/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// === CONFIGURATION ===
const int numChannels = 16;
Adafruit_ADS1115 adsModules[4];
uint8_t i2cAddresses[4] = {0x48, 0x49, 0x4A, 0x4B};

// === SETUP ===
void setup() {
  Serial.begin(115200);          // Fast baud rate for data throughput
  Wire.begin();                  // I2C: SDA = 20, SCL = 21 on Arduino Mega

  Serial.println("START");       // Used by Python script to sync

  // Initialize all 4 ADS1115 modules
  for (int i = 0; i < 4; i++) {
    adsModules[i] = Adafruit_ADS1115();
    if (!adsModules[i].begin(i2cAddresses[i])) {
      Serial.print("ADS1115 at 0x");
      Serial.print(i2cAddresses[i], HEX);
      Serial.println(" not found!");
    }
    adsModules[i].setGain(GAIN_TWO);  // //TWO: ±2.048 V range, ONE: ±4.096 V range, FOUR: ±1.024 V range
    adsModules[i].setDataRate(RATE_ADS1115_128SPS);  // samples per second setter
  }
}

// === LOOP ===
void loop() {
  static unsigned long lastSample = 0;
  unsigned long now = millis();

  if (now - lastSample >= 100) {  // 10 Hz sampling rate
    lastSample = now;

    Serial.print(now);  // Timestamp in ms

    int sensorIndex = 0;
    for (int module = 0; module < 4; module++) {
      for (int channel = 0; channel < 4; channel++) {
        int16_t raw = adsModules[module].readADC_SingleEnded(channel);
        Serial.print(",");
        Serial.print(raw);
        sensorIndex++;
      }
    }

    Serial.println();  // End of line = one full row
  }
}
