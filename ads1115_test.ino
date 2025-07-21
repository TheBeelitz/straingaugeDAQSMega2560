/*  ADS1115 4-Ch 16-bit ADC Test Program
 
    Basic code to test the ADS1115 ADC Module.  Takes a single-ended reading
    on module channel A0.  Uses default measurement range of of 6.144V 

    Module connections:
      VDD = 5V or 3.3V to match microcontroller
      Gnd = Ground
      SCL = SCL / A5 on microcontroller
      SDA = SDA / A4 on microcontroller
      ADDR = Board has pull-down to ground (sets I2C address of 0x48)
      A0-A3 = 3.3V on MCU.  Voltage to be measured. Can be any voltage <= VDD

    For 16-bit vs 12-bit verification, connect input to ground and verify the 
    minimum reading is less than 3mV.  Readings should change in approximately
    200uV (0.0002V) steps
*/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  

void setup(void)
{
  Serial.begin(9600);

  Serial.println("Read single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 0.1875mV");

  // Use !ads.begin(0x48, &Wire1); to specify using I2C1 instead of I2C
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
}

void loop(void)
{
  int16_t adc0, adc1, adc2, adc3;
  float volts0, volts1, volts2, volts3;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);
  volts2 = ads.computeVolts(adc2);
  volts3 = ads.computeVolts(adc3);

  Serial.println("-----------------------------------------------------------");

  Serial.print("AIN0: ");Serial.print(volts0, 4); Serial.println("V");
  Serial.print("AIN1: ");Serial.print(volts1, 4); Serial.println("V");
  Serial.print("AIN2: ");Serial.print(volts2, 4); Serial.println("V");  
  Serial.print("AIN3: ");Serial.print(volts3, 4); Serial.println("V");
  Serial.println();
  delay(1000);
}