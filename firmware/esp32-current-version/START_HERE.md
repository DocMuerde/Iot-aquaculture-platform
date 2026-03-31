# START HERE – ESP32 Firmware (Current Version)

This document provides the minimum steps required to compile, upload, and run the current firmware version of the IoT aquaculture platform.

---

## 1. Required Environment

### IDE
- Arduino IDE (recommended)

### Board Package
- Install ESP32 board support:
  - Go to: **File → Preferences**
  - Add to “Additional Board Manager URLs”:
    ```
    https://dl.espressif.com/dl/package_esp32_index.json
    ```
  - Then: **Tools → Board → Boards Manager → install ESP32**

### Board Selection
- Select your board (depending on your hardware):
  - Example: `ESP32 Dev Module`
  - Or your specific board (e.g., DFRobot FireBeetle ESP32)

---

## 2. Required Libraries

Install the libraries in arduino_definitivo_v8.ino

---

## 3. Project Structure

Open the file: Acuacol_definitivo_V8.ino


All `.ino` files in the folder are compiled together.

Main functional blocks:

- Sensor acquisition
- RTC and time handling
- SD storage
- MQTT communication
- Firebase communication
- LCD interface and menu system
- pH control logic
- Interrupt handling (buttons via TCA9555)

---
## 4. Configuration

### SD Card Configuration File

A file named: config.txt

must be present on the microSD card.

It contains:
- WiFi credentials
- calibration parameters
- installation ID
- pH control parameters

If the file is not found:
- default values are loaded
- system continues operating

---

### Firebase Configuration

Defined in: config_firebase.h


⚠️ Important:
- Contains API key and project ID
- Should be modified before deployment
- Do NOT share real credentials publicly

---

## 6. Setup Sequence (What Happens on Boot)

The firmware performs the following steps:

1. Serial initialization
2. Interrupt setup (TCA9555 → ESP32)
3. I2C bus initialization
4. ADC (ADS1115) initialization
5. LCD initialization
6. RTC initialization
7. SD card initialization
8. Load configuration from SD
9. WiFi connection attempt
10. RTC synchronization (NTP → RTC)
11. MQTT setup
12. Sensor initialization (DHT22, DS18B20)
13. Creation of CSV log file
14. Pump/servo initialization

---

## 7. Main Loop Behavior

The system runs continuously performing:

### Periodic tasks
- Sensor acquisition
- LCD update
- Data logging to SD
- MQTT transmission
- Firebase transmission

### Event-based tasks
- Button handling (via interrupt flag)
- Menu navigation
- Calibration routines
- Manual/automatic pH control

### Fault tolerance
- If communication fails:
  - data is stored locally
  - resent later

---

## 8. pH Control System

The firmware includes a simple dosing control:

### Parameters
- `PHref` → target pH
- `KPH` → proportional gain
- `ContPH` → enable/disable control

### Operation
- Computes error: `PHref - measured pH`
- Converts error into pump activation time
- Activates relay/servo

### Modes
- Manual (from LCD)
- Automatic

---

## 9. Important Notes

- The ESP32 ADC is NOT used for critical analog measurements  
  → external ADC (ADS1115) is used instead

- All modules communicate via I2C  
  → ensure proper wiring and pull-up resistors

- Interrupt handling is minimal (flag-based)  
  → actual logic runs in the main loop

---

## 10. Known Limitations

- Heavy use of global variables
- Tight coupling between modules
- Mixed responsibilities across `.ino` files
- ESP32 limitations:
  - ADC nonlinearity
  - interrupt reliability with I/O expander

---

## 11. Future Version

A new firmware version is planned with:

- Arduino UNO R4 (RA4M1)
- SIM900 cellular communication
- Improved modular architecture

---

## 12. Quick Start Checklist

Before uploading:

✔ Board selected correctly  
✔ Libraries installed  
✔ `config.txt` present on SD  
✔ WiFi credentials configured  
✔ Firebase configuration updated  
✔ Sensors connected  
✔ I2C bus verified  

---

## 13. First Test

After uploading:

1. Open Serial Monitor
2. Check:
   - WiFi connection
   - RTC time
   - sensor readings
3. Verify LCD output
4. Check SD file creation
5. Confirm MQTT/Firebase transmission

---

## Contact / Maintainer

Ignacio Alvarado Aldea  
University of Seville  
Department of Systems Engineering and Automation



