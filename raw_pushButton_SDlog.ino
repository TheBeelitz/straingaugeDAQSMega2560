#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_ADS1X15.h>
#include <avr/wdt.h>

const int numChannels = 16;
const int buttonPin = 2;
const int maxCycles = 9;
const int chipSelect = 53;  // SD card CS pin (for Arduino Mega)

int testNumber = 1;
int cycleNumber = 0;
bool testActive = false;
bool cycleInProgress = false;

int lastButtonState = HIGH;
unsigned long lastSample = 0;
const unsigned long sampleInterval = 10;  // Aim for faster logging (adjust as needed)

Adafruit_ADS1115 adsModules[4];
uint8_t i2cAddresses[4] = {0x48, 0x49, 0x4A, 0x4B};

File dataFile;
char testFolder[20];
char filename[32];

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

    if (!SD.begin(chipSelect)) {
        Serial.println("⚠ SD card initialization failed!");
        while (1);
    }

    for (int i = 0; i < 4; i++) {
        adsModules[i] = Adafruit_ADS1115();
        if (!adsModules[i].begin(i2cAddresses[i])) {
            Serial.print("ADS1115 at 0x");
            Serial.print(i2cAddresses[i], HEX);
            Serial.println(" not found!");
            while (1);
        }
        adsModules[i].setGain(GAIN_TWO);
        adsModules[i].setDataRate(RATE_ADS1115_860SPS);  // Fastest setting
    }

    Serial.println("READY - Press button to start test");
}

void loop() {
    wdt_reset();

    int buttonState = digitalRead(buttonPin);

    if (buttonState == LOW && lastButtonState == HIGH) {
        delay(50);

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
    snprintf(testFolder, sizeof(testFolder), "test_%d", testNumber);

    if (!SD.mkdir(testFolder)) {
        Serial.println("⚠ Failed to create test folder!");
        testActive = false;
        return;
    }

    Serial.print("Test started: ");
    Serial.println(testFolder);
    Serial.println("Press button to start cycle 1...");
}

void startCycle() {
    Serial.print("Starting cycle ");
    Serial.println(cycleNumber);

    snprintf(filename, sizeof(filename), "%s/cycle_%d.bin", testFolder, cycleNumber);
    dataFile = SD.open(filename, FILE_WRITE);

    if (!dataFile) {
        Serial.println("⚠ Failed to open file for writing");
        cycleInProgress = false;
        return;
    }

    cycleInProgress = true;
    lastSample = millis();
}

void endCycle() {
    cycleInProgress = false;

    if (dataFile) {
        dataFile.close();
    }

    Serial.print("Cycle ");
    Serial.print(cycleNumber);
    Serial.println(" complete. Press button for next cycle.");
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
    static int flushCounter = 0;
    int16_t rawData[numChannels];
    int sensorIndex = 0;

    for (int module = 0; module < 4; module++) {
        for (int channel = 0; channel < 4; channel++) {
            rawData[sensorIndex++] = adsModules[module].readADC_SingleEnded(channel);
        }
    }

    if (dataFile) {
        uint32_t timestamp = millis();
        dataFile.write((uint8_t*)&timestamp, sizeof(timestamp));
        dataFile.write((uint8_t*)&rawData, sizeof(rawData));

        flushCounter++;
        if (flushCounter >= 10) {
            dataFile.flush();
            flushCounter = 0;
        }
    }

    Serial.println("Sample saved (binary)");
}
