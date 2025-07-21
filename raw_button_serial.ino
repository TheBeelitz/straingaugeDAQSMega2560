#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <avr/wdt.h>

const int numChannels = 16;
const int buttonPin = 2;
const int maxCycles = 7;

int testNumber = 1;
int cycleNumber = 0;
bool testActive = false;
bool cycleInProgress = false;
String testFolder;

int lastButtonState = HIGH;
unsigned long lastSample = 0;

Adafruit_ADS1115 adsModules[4];
uint8_t i2cAddresses[4] = {0x48, 0x49, 0x4A, 0x4B};

const unsigned long sampleInterval = 125;  // ~8Hz total sampling
float gainFactors[numChannels];  // Optional gain scaling

void sampleSensors();
void startTest();
void startCycle();
void endCycle();
void endTest();

void setup() {
    Serial.begin(115200);
    Wire.begin();
    pinMode(buttonPin, INPUT_PULLUP);
    wdt_enable(WDTO_1S);

    // Initialize ADS1115 modules
    for (int i = 0; i < 4; i++) {
        adsModules[i] = Adafruit_ADS1115();
        if (!adsModules[i].begin(i2cAddresses[i])) {
            Serial.print("ADS1115 at 0x");
            Serial.print(i2cAddresses[i], HEX);
            Serial.println(" not found!");
            while (1);
        }
        adsModules[i].setGain(GAIN_ONE);  // ±4.096 V range
        adsModules[i].setDataRate(RATE_ADS1115_128SPS);
    }

    for (int i = 0; i < numChannels; i++) {
        gainFactors[i] = 1.0;  // Adjust if needed
    }

    // Print header
    Serial.print("Time(ms)");
    for (int i = 0; i < numChannels; i++) {
        Serial.print(",CH");
        if (i < 9) Serial.print("0");
        Serial.print(i + 1);
    }
    Serial.println();

    Serial.println("READY - Press button to start test");
}

void loop() {
    wdt_reset();

    int buttonState = digitalRead(buttonPin);

    if (buttonState == LOW && lastButtonState == HIGH) {
        delay(50);  // debounce

        if (!testActive) {
            startTest();
        } else if (testActive && !cycleInProgress && cycleNumber <= maxCycles) {
            startCycle();
        } else if (testActive && cycleInProgress) {
            endCycle();
            if (cycleNumber > maxCycles) {
                endTest();
            }
        }

        delay(200);
    }

    lastButtonState = buttonState;

    if (testActive && cycleInProgress) {
        unsigned long now = millis();
        if (now - lastSample >= sampleInterval) {
            lastSample = now;
            sampleSensors();
        }
    }
}

void startTest() {
    testActive = true;
    cycleNumber = 1;
    testFolder = "test_" + String(testNumber);
    Serial.println("Test started: " + testFolder);
    Serial.println("Press button to start cycle 1...");
}

void startCycle() {
    Serial.println("Starting cycle " + String(cycleNumber) + " in " + testFolder);
    cycleInProgress = true;
    lastSample = millis();  // Reset timer
}

void endCycle() {
    cycleInProgress = false;
    Serial.println("Cycle " + String(cycleNumber) + " complete. Press button for next cycle.");
    cycleNumber++;
}

void endTest() {
    Serial.println("⚡ Test Completed: All cycles finished.");
    Serial.println("Preparing for next test...");
    cycleNumber = 0;
    testNumber++;
    testActive = false;
    cycleInProgress = false;
    Serial.println("READY - Press button to start next test.");
}

void sampleSensors() {
    float readings[numChannels];
    int sensorIndex = 0;

    for (int module = 0; module < 4; module++) {
        for (int channel = 0; channel < 4; channel++) {
            int16_t raw = adsModules[module].readADC_SingleEnded(channel);
            float voltage = adsModules[module].computeVolts(raw);
            readings[sensorIndex] = voltage * gainFactors[sensorIndex];
            sensorIndex++;
        }
    }

    Serial.print(millis());  // Timestamp
    for (int i = 0; i < numChannels; i++) {
        Serial.print(",");
        Serial.print(readings[i], 6);
    }
    Serial.println();
}
