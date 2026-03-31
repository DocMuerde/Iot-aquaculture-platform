# ESP32 Current Firmware Version

## Overview

This folder contains the current operational firmware of the IoT aquaculture platform based on an **ESP32** and developed with the **Arduino IDE**.

The firmware is responsible for:

- Acquiring data from water and environmental sensors
- Managing local configuration from a microSD card
- Displaying system status and calibration menus on a 20x4 I2C LCD
- Logging data locally to the microSD card
- Sending measurements to a remote MQTT server
- Sending measurements to Firebase / Firestore
- Handling local pH control through a dosing pump
- Managing user interaction through buttons connected via an I2C I/O expander

This version corresponds to the current ESP32-based hardware generation and is kept in its original structure to preserve stability and reproducibility.

---

## Development Environment

- **IDE:** Arduino IDE
- **Target platform:** ESP32
- **Current controller board:** ESP32-based main board
- **Associated peripheral board:** I2C interface board with:
  - ADS1115 ADC
  - TCA9555 I/O expander
  - DS3231 RTC
  - 20x4 I2C LCD

---

## Main Firmware Architecture

The code is organized as a main `.ino` file plus multiple auxiliary `.ino` files, all compiled together by the Arduino IDE.

### Main runtime tasks

The firmware performs four main activities:

1. **Sensor acquisition**
2. **User interface and calibration**
3. **Local storage**
4. **Remote communication**

In addition, it implements a basic **automatic pH control strategy** based on a dosing pump.

---

## Main File

### `Acuacol_definitivo_V8.ino`

This is the main entry point of the firmware.

It defines:

- Included libraries
- Hardware pin mapping
- Global objects
- Global variables
- Configuration structure
- Time constants
- Setup procedure
- Main loop

### Main responsibilities

#### `setup()`
Initializes the complete system:

- Serial port
- External interrupt for button events
- ADS1115 ADC
- I2C bus
- LCD
- TCA9555 digital I/O expander
- Relay default states
- microSD card
- Configuration loading from `/config.txt`
- WiFi
- RTC
- MQTT
- DHT22 sensors
- DS18B20 sensor bus
- CSV logging file
- Servo-controlled dosing pump

#### `loop()`
Implements the cyclic operation of the device:

- Handles button events
- Updates the LCD
- Periodically acquires sensor values
- Periodically sends data
- Stores data locally when communication fails
- Sends data to Firebase
- Writes measurements to the SD card
- Executes automatic pH pump control when enabled

---

## Configuration and Time Management

### `SD_operations.ino`

This file manages local storage and persistent configuration.

#### `Write_SD()`
Writes one measurement record to the main CSV file on the SD card.

Stored fields include:

- timestamp
- date and time
- water temperatures
- pH
- dissolved oxygen
- conductivity
- environmental temperature and humidity
- pH control state
- pump actuation time
- communication status flag

#### `appendFile(fs::FS &fs, const char * path, String message)`
Appends a line of text to a file in the SD card.

Used for:

- measurement logging
- buffering unsent data
- Firebase pending queue

#### `loadConfiguration(const char *filename)`
Loads system configuration from `/config.txt`.

If the file cannot be read, it loads a default configuration, including:

- WiFi credentials
- sensor calibration parameters
- installation ID
- pH control parameters

#### `saveConfiguration(const char *filename)`
Saves the current configuration to `/config.txt`.

Used after calibration or parameter adjustment through the local interface.

#### `printFile(const char *filename)`
Prints the contents of a file to the serial port.

#### `p_config()`
Prints the current loaded configuration to the serial monitor.

---

### `Get_Time.ino`

#### `Get_time()`
Reads the current date and time from the RTC and updates:

- Unix timestamp
- date string
- time string
- hour, minute, second, day, month, year

Also applies manual time-zone correction using the configured daylight-saving rules.

---

### `Cal_RTC.ino`

#### `Cal_RTC()`
Synchronizes the DS3231 RTC from NTP time after WiFi connection.

It:
- gets the current network time
- writes it to the RTC
- shows the update on the LCD

---

## Sensor Acquisition

### `Get_Sensors.ino`

#### `Get_Sensors()`
Central acquisition wrapper.

Calls all sensor-reading routines and updates:

- water temperatures
- pH
- conductivity
- dissolved oxygen
- environmental temperature and humidity
- float switch states

---

### `Get_Tw_value.ino`

#### `Get_Tw_value()`
Reads the **DS18B20** water temperature sensor.

Updates:
- `Tw2`

If the sensor read fails (`-127.0`), the stored value is not overwritten.

---

### `Get_env_values.ino`

#### `Get_env_values()`
Reads both **DHT22** sensors.

Updates:
- `Ta1`, `Ha1`
- `Ta2`, `Ha2`

If a read fails, an error is printed to serial and the previous valid value is preserved.

---

### `Get_pH_value.ino`

#### `Get_pH_value()`
Reads the pH analog channel from the **ADS1115** and computes the pH value.

Main processing steps:

- acquires 10 raw samples
- sorts the samples
- discards extreme values
- averages the remaining samples
- applies an additional FIFO moving average filter
- converts ADC reading to voltage
- applies linear calibration using two calibration points

Updates:
- raw and filtered pH internal variables
- calibration voltage
- final `PH`

---

### `Get_o2_value.ino`

#### `Get_o2_value()`
Reads the dissolved oxygen analog channel from the **ADS1115** and computes oxygen concentration.

Main processing steps:

- calculates the saturation voltage from calibration data
- reads the ADC value
- applies moving average filtering
- converts to voltage
- compensates using the water temperature
- converts the result to dissolved oxygen concentration

Updates:
- `o2_voltage`
- `o2_voltage_cal`
- `O2`

---

### `Get_EC_value.ino`

#### `Get_EC_value()`
Reads conductivity and the auxiliary analog temperature associated with the conductivity probe.

Main processing steps:

- reads auxiliary analog temperature channel
- filters the value
- converts probe voltage to temperature
- reads conductivity voltage
- computes conductivity in `uS/cm`

Updates:
- `Tw1`
- `te_voltage`
- `ec_voltage`
- `EC`

#### `EC_convVoltagetoTemperature_C(float voltage)`
Converts the probe temperature channel voltage into temperature in °C.

#### `EC_getEC_us_cm(float voltage, float temperature)`
Computes electrical conductivity in `uS/cm` from sensor voltage and temperature compensation.

#### `EC_calibrate(float voltage, float temperature)`
Used during conductivity calibration to estimate the probe constant.

> Note: part of the conductivity logic is derived from or adapted from the DFRobot EC Pro approach, but implemented directly in this firmware rather than using the original library.

---

## Communication

### `setup_wifi.ino`

#### `setup_wifi()`
Connects the ESP32 to the configured WiFi network.

Main actions:

- shows status on LCD
- tries to connect using credentials from configuration
- stops after a timeout
- updates the WiFi LED state
- prints connection information to serial

---

### `setup_mqtt.ino`

#### `setup_mqtt()`
Connects the device to the MQTT broker.

Main actions:

- configures server and port
- retries until timeout
- updates MQTT LED state
- returns success or failure status

---

### `Send_MQTT.ino`

#### `Send_MQTT()`
Builds a JSON payload with the current measurements and publishes it to the MQTT broker.

It includes:

- installation ID
- timestamp
- water variables
- environmental variables
- float switch states

If there are previously unsent records in the SD card, it also attempts to resend them.

#### `Store_MQTT()`
Stores the MQTT JSON payload in a local SD file when it cannot be sent immediately.

#### `Send_Stored_DATA(char * inst, const char * path)`
Attempts to resend previously buffered MQTT messages stored on the SD card.

If all buffered messages are successfully sent, the pending file can be removed.

---

### `Send_Firebase.ino`

Implements a second communication path using the Firestore REST API.

#### `Send_Firebase()`
Sends the current measurements to Firebase / Firestore.

If WiFi is unavailable or sending fails, the data is stored locally for later retry.

#### `buildFirestoreJSON()`
Builds the Firestore-compatible JSON payload.

#### `Store_Firebase_SD()`
Stores one failed Firebase transmission in a pending SD file.

#### `Resend_Firebase_Pending()`
Attempts to resend all pending Firebase records stored on the SD card.

#### `Resend_Single_Firebase(String csvData)`
Rebuilds and resends one previously stored Firebase record.

---

## User Interface and Calibration

### `Show_LCD.ino`

#### `Show_LCD()`
Updates the 20x4 LCD according to the current operating screen.

Two main modes are supported:

- **Normal monitoring screen**
  - alternates between water variables and environmental variables

- **Calibration / configuration screens**
  - pH calibration
  - dissolved oxygen calibration
  - conductivity calibration
  - pH automatic/manual control
  - pH control gain
  - pH setpoint

---

### `pantallas.ino`

#### `pantallas()`
Handles button actions and the logic of the local menu system.

It:
- reads buttons via the TCA9555
- changes the active screen
- updates temporary calibration parameters
- saves modified parameters to `/config.txt`
- controls the pump manually when manual mode is selected
- configures automatic pH control parameters

This is the main interaction layer between the user and the embedded configuration system.

---

## Interrupt Handling

### `ISR.ino`

#### `ISR()`
Interrupt service routine for button activity.

It only sets a flag:
- `flag = true`

The actual processing is deferred to the main loop.

This is a good design choice because it keeps the ISR short.

---

## Utility Functions

### `String_op.ino`

Contains helper functions for string manipulation and serial output.

#### `SerialString(String cad)`
Prints a `String` character by character to the serial monitor.

#### `reemplaza(char * cad, char * in, char * rep)`
Searches for a substring and replaces it with another substring.

Used to replace `"null"` formatting artifacts in generated JSON payloads.

#### `Borra_char(char * in, int i)`
Deletes one character from a C-style string.

#### `inserta_char(char * in, int i, char c)`
Inserts one character into a C-style string.

---

### `write_serial.ino`

#### `write_serial()`
Legacy debugging function used to print filtered and unfiltered sensor values to serial.

Currently appears to be unused.

---

## Control Functionality

### Automatic pH Control
The firmware includes a simple dosing strategy for pH correction.

The control logic is implemented mainly in the main loop and configured from the LCD menu.

Main configurable parameters:

- `KPH`: proportional relation between pH error and pump ON time
- `PHref`: target pH
- `ContPH`: enables/disables automatic control

Operation:

- if pH is sufficiently below the reference, the firmware computes a pump activation time
- the pump is driven through a relay / servo-based actuation mechanism
- manual control is also available from the menu

---

## Current File Summary

### Core files
- `Acuacol_definitivo_V8.ino`
- `Get_Sensors.ino`
- `Get_Time.ino`
- `Cal_RTC.ino`

### Sensor files
- `Get_Tw_value.ino`
- `Get_env_values.ino`
- `Get_pH_value.ino`
- `Get_o2_value.ino`
- `Get_EC_value.ino`

### Communication files
- `setup_wifi.ino`
- `setup_mqtt.ino`
- `Send_MQTT.ino`
- `Send_Firebase.ino`

### Storage files
- `SD_operations.ino`

### User interface files
- `Show_LCD.ino`
- `pantallas.ino`

### Support files
- `ISR.ino`
- `String_op.ino`
- `write_serial.ino`

### Configuration file
- `config_firebase.h`

---

## Known Limitations

- The firmware relies heavily on global variables shared across `.ino` files.
- The current structure is functional but not yet refactored into `.cpp/.h` modules.
- Some credentials and service parameters are stored directly in source/configuration files.
- The current version is tied to the ESP32 generation of the hardware.
- The next firmware generation is expected to migrate to:
  - **Arduino UNO R4**
  - **SIM900-based cellular communication**

---

## Security Note

Before sharing this repository with other collaborators, it is strongly recommended to remove or externalize:

- WiFi credentials
- MQTT credentials
- Firebase API keys
- server IP addresses
- any deployment-specific identifiers

A future refactoring should replace these values with template configuration files such as:

- `config.example.h`
- `config.txt.example`

---

## Recommended Next Step

For the next hardware/firmware generation, a cleaner architecture is recommended, with:

- modular `.cpp/.h` files
- separated communication backends
- explicit configuration management
- improved onboarding documentation
- support for the Arduino R4 + SIM module version
