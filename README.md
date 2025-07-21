
# Arduino Script for Strain Data Aqcuisition – Manual Balancing - Push Button Controlled SD Logging

## 1. Sketchbook Overview
### 1.1 Main sketchbooks 
This project consists of two *main* Arduino sketches for actual test procedures:

- **balancing.ino** – Reads and prints data from strain sensors to serial monitor to allow manual quarter wheatstone bridge balancing.

- **raw_pushButton_SDlog.ino** – Logs raw binary sensor data using a button-triggered procedure to an SD card.



### 1.2 Component testing sketchbooks
The physical setup required *component testing* to ensure its expected behaviour:

- **INA333_test.ino** – This code reads the analog voltage from the *VOUT pin* of an INA333 (CJMCU-333) module using Arduino's analog input A0. The INA333 amplifies the small differential signal from a strain gauge (or test voltage) and outputs a voltage centered around REF (1.65V default).  

- **ic2scan.ino** – This code tests the primary ic2 connection *0x+, expecting a connected device. This scanner allows for a primary check up of the functionality of the ic2 communication.

- **ads1115_test.ino** – Basic code to test the ADS1115 ADC Module.  Takes a single-ended reading on module Arduino channel *A0*.  Uses default measurement range of of 6.144V.

- **rawdata_reading.ino** – Full-scale 16-channel strain gauge measurement using the INA333 + ADS1115 with continuous serial print for visual check up.

- **raw_button_serial.ino** – Full-scale 16-channel strain gauge measurement using the INA333 + ADS1115 with push button driven script and raw data to serial print. Testing functionality of push button module.


---

## 2. Requirements

### 2.1 Setup Description
- This Arduino project is designed to be run with an Arduino Mega 2560. 
- The sketches are uploaded sequentially and run the same principle functions, reading binary raw data from strain gauges. 
- Strain gauge values are amplified by an INA333 amplifier module. 
- The INA333 modules are followed by analog to digital converter modules, namely ADS1115. 
- The ADS modules communicate via I2C protocol with the Arduino. Limiting the setup to four ADS Modules with their specifc number of analog pins. 
- The ADS1115 is capable of reading four analog inputs at once. Thus, the setup handles up to 16 strain gauge readings at once. 
- Each strain gauge is placed in a quarter wheatstone bridge constellation. With two resistors and a potentiometer across the bridge to allow for manual balancing pre testing, ensuring optimal range capability for the readings.
- The main script **raw_pushButton_SDlog.ino** is controlled by manual push button input, stepping through the expected test protocol behaviour.
- The setup requires an SD card logging module to ensure full data logging and saving during the execution of the script.

### 2.2 Hardware Components
- Arduino Mega 2560
- 4 ADS1115 ADC Modules
- 16 INA333 Amplifier Modules (CJMCU333)
- 16 Strain Gauges in a quarter Wheatstone Bridge configuration
- SD Card Module HW-125 + microSD card
- Push Button Module PCB0116

### 2.3 Software & Libraries
- Arduino IDE (version 2.3 or later)
- Required Libraries:
  - Arduino.h
  - Wire.h
  - SPI.h
  - SD.h
  - Adafruit_ADS1x15.h
  - avr/wdt.h
  - TimerOne.h

## 3. Installation & Reproduction Step for Strain Test Procedure
### 3.1 Acquire files
- Clone this repository:
   ```
   git clone https://github.com/TheBeelitz/straingaugeDAQSMega2560.git
   ```
- Or request ZIP.folder from beelitz@tu-berlin.de with all required .ino files

### 3.2 Execution Advice
- Open **raw_pushButton_SDlog.ino** within in your Arduino IDE, get familiar with the coding style and functions
- Install required libraries, any library within the project is necessary to run this sketchbook, once installed all other sketchbooks should run wo problems
- All sketchbooks are designed to be run in the serial monitor environment thus observation of serial output is required
- To test all setup components run the -test.ino files as in aforementioned order ##1.2 

### 3.3 run balancing.ino
Open **balancing.ino** within in your Arduino IDE, get familiar with the coding style and functions

#### 3.3.1 balancing.ino Description
This sketch reads data from strain sensors, continously prints voltage data of the sensors individual wheatstone bridge to serial monitor, allowing test conductor to balance wheatstone bridges manually through potentiometers. Depending on range of expected strain and resulting voltage adjust ADS setGain function. The expected balanced voltage depends on voltage applied on the INA333 *V_ref* pin. For this case internal *V_ref = 1.6V* was used.

#### 3.3.2 balancing.ino Key Functions 

##### 3.3.2.1 From Libraries
Serial.begin(115200), adsModules[i].setGain(GAIN_ONE), adsModules[i].setDataRate(RATE_ADS1115_860SPS)

##### 3.3.2.2 Sketchbook
setup,loop

#### 3.3.3 balancing.ino Execution
1. Test assembly of hardware, ensuring all strain gauges are connected
2. Upload `balancing.ino` to the Arduino
3. Observe balance correction in real-time on serial monitor
4. Adjust channel by channel using potentiometer

#### 3.3.4 balancing.ino Example Serial Print

Balancing Wheatstone Bridge...
System Ready: Adjust potentiometers to achieve 1.6V output.

Voltages: 1.587500   1.590000   1.592500   1.595000   1.597500   1.600000   1.602500   1.605000   1.607500   1.610000   1.612500   1.587500   1.590000   1.592500   1.595000   1.597500   


---

### 3.4 run raw_pushButton_SDlog.ino
Open **raw_pushButton_SDlog.ino** within in your Arduino IDE, get familiar with the coding style and functions

#### 3.3.1 raw_pushButton_SDlog.ino Description
This sketch logs raw sensor data to an SD card. The push button activates a predefined test procedure, going through a number of cycles that were required in this test case. Data is written to a file on the SD card with cycles as files within folder. 


#### 3.3.2 raw_pushButton_SDlog.ino Key Functions

##### 3.3.2.1 From Libraries
Serial.begin(115200), adsModules[i].setGain(GAIN_ONE), adsModules[i].setDataRate(RATE_ADS1115_860SPS), digitalRead(buttonPin), pinMode(), wdt_enable()

##### 3.3.2.2 Sketchbook
sampleSensors, startTest, startCycle, endCycle, endTest, setup, loop

#### 3.4.3 raw_pushButton_SDlog.ino Execution
1. Test assembly of hardware, ensuring all strain gauges are connected
2. Check SD card formatting and clear all previous measurements, insert FAT32 microSD into module
3. Upload `raw_pushButton_SDlog.ino` to the Arduino
4. Observe balance serial monitor for `READY - Press button to start test`
5. Press push button when ready to start `Warm-Up` Cycle (=Cycle0), meant to settle in the system to stabilize readings, previously done for about 30secs, used as a baseline for post script zero-offsetting/calibration of channels
6. To start logging for a test cycle press button, to finish logging of a test cycle press button
7. Script is designed to run 9 cycles, cycle 0 = Warm-Up, cycles 1-8 = actual test cycles, if stopped by malfunction or user fault, data will still be saved to SD card
8. After successful procedure, serial print shows: `⚡ Test Completed: All cycles finished.`, `Preparing for next test...`allowing for contious testing of multiple tests after one another - **However, I advise to check the successful data logging in between tests.**

#### 3.3.4 braw_pushButton_SDlog.ino Example Serial Print
READY - Press button to start test

Test started: test_1
Press button to start cycle 1...

Starting cycle 1
Sample saved (binary)
Sample saved (binary)
...

Cycle 1 complete. Press button for next cycle.

Starting cycle 2
Sample saved (binary)
Sample saved (binary)
...

Cycle 2 complete. Press button for next cycle.

Starting cycle 3
Sample saved (binary)
Sample saved (binary)
...
Cycle 3 complete. Press button for next cycle.

Starting cycle 4
Sample saved (binary)
Sample saved (binary)
...
Cycle 4 complete. Press button for next cycle.

Starting cycle 5
Sample saved (binary)
Sample saved (binary)
...
Cycle 5 complete. Press button for next cycle.

Starting cycle 6
Sample saved (binary)
Sample saved (binary)
...
Cycle 6 complete. Press button for next cycle.

Starting cycle 7
Sample saved (binary)
Sample saved (binary)
...
Cycle 7 complete. Press button for next cycle.

Starting cycle 8
Sample saved (binary)
Sample saved (binary)
...
Cycle 8 complete. Press button for next cycle.

Starting cycle 9
Sample saved (binary)
Sample saved (binary)
...
Cycle 9 complete. Press button for next cycle.

⚡ Test Completed: All cycles finished.
Preparing for next test...
READY - Press button to start next test.


## License
This project is published under CC BY 4.0 standards.
https://creativecommons.org/licenses/by/4.0/


