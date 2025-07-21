#include <Arduino.h>

const int analogPin = A0;         // INA333 VOUT connected to A0
const float analogRef = 5.0;      // Arduino analog reference voltage
const float vref = 1.65;          // INA333 internal reference voltage
const float gain = 501.0;         // INA333 gain — adjust this if different
const int resolution = 1023;      // 10-bit ADC

void setup() {
  Serial.begin(115200);
  analogReference(DEFAULT);
  Serial.println("Bridge Balance Monitor Started");
}

void loop() {
  int raw = analogRead(analogPin);
  float vout = (raw * analogRef) / resolution;
  float diffFromVREF = vout - vref;
  float vin_diff = diffFromVREF / gain;

  Serial.print("VOUT: ");
  Serial.print(vout, 5);
  Serial.print(" V | Offset from VREF: ");
  Serial.print(diffFromVREF * 1000, 3);
  Serial.print(" mV | Estimated ΔVIN: ");
  Serial.print(vin_diff * 1000, 3);
  Serial.println(" mV");

  delay(100);
}
