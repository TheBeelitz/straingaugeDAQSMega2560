#include <Wire.h>
#include <Adafruit_ADS1X15.h>

const int numChannels = 16;
Adafruit_ADS1115 adsModules[4];
uint8_t i2cAddresses[4] = {0x48, 0x49, 0x4A, 0x4B};

void setup() {
    Serial.begin(115200);
    Wire.begin();

    Serial.println("Balancing Wheatstone Bridge...");

    for (int i = 0; i < 4; i++) {
        adsModules[i] = Adafruit_ADS1115();
        if (!adsModules[i].begin(i2cAddresses[i])) {
            Serial.print("ADS1115 at 0x");
            Serial.print(i2cAddresses[i], HEX);
            Serial.println(" not found!");
            while (1);
        }
        adsModules[i].setGain(GAIN_ONE);
        adsModules[i].setDataRate(RATE_ADS1115_860SPS);
    }

    Serial.println("System Ready: Adjust potentiometers to achieve 2.5V output.");
}

void loop() {
    static unsigned long lastSample = 0;
    unsigned long now = millis();

    if (now - lastSample >= 100) {  
        lastSample = now;

        Serial.print("Voltages: ");

        int sensorIndex = 0;
        for (int module = 0; module < 4; module++) {
            for (int channel = 0; channel < 4; channel++) {
                int16_t raw = adsModules[module].readADC_SingleEnded(channel);
                float voltage = adsModules[module].computeVolts(raw);

                Serial.print(voltage, 6);
                Serial.print("   ");  // Three spaces for better readability
                
                // **Warning for readings close to 4.9V**
                if (voltage >= 4.9) {
                    Serial.print("⚠ WARNING: High Voltage! ");
                }
                
                sensorIndex++;
            }
        }

        Serial.println();  
    }
}
