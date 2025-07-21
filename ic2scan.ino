#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("I2C Scanner:");
}

void loop() {
  byte error, address;
  int devices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      Serial.println(address, HEX);
      devices++;
    }
  }

  if (devices == 0)
    Serial.println("No I2C devices found.");

  delay(3000);
}
