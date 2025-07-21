 /*
This code reads the analog voltage from the **VOUT pin** of an INA333 (CJMCU-333) module  
using Arduino’s analog input A0. The INA333 amplifies the small differential signal  
from a strain gauge (or test voltage) and outputs a voltage centered around REF (1.65 V default).  

Module connections (CJMCU-333):  
  VIN+   = Positive signal input (e.g., 2.510 V)  
  VIN–   = Negative signal input (e.g., 2.500 V)  
  VOUT   = Amplifier output → Connect to A0  
  VREF   = Internal 1.65 V reference (via voltage divider)  
  VCC    = 5V from Arduino  
  GND    = Arduino GND  

How it works:  
  - The INA333 output reflects:  
      VOUT = VREF + Gain × (VIN+ - VIN–)  
  - Readings should change smoothly with small changes in differential input  
  - Confirm functionality by applying a known small voltage difference and  
    verifying expected output using Serial Monitor  

*/
#include <Arduino.h>

const int analogPin = A0;  // Connect to INA333 VOUT
float referenceVoltage = 5.0;  // Arduino analog reference
int analogResolution = 1023;   // 10-bit resolution

void setup() {
  Serial.begin(115200);
  analogReference(DEFAULT);  // 5V ref
  Serial.println("INA333 Analog Output Test Started");
}

void loop() {
  int raw = analogRead(analogPin);
  float voltage = (raw * referenceVoltage) / analogResolution;

  Serial.print("Raw ADC: ");
  Serial.print(raw);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 4);
  Serial.println(" V");

  ;
}
