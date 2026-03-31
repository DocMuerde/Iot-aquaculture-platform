# System Schematics

## Overview
This folder contains the electrical schematics of the IoT aquaculture platform.

The system is composed of two main boards:

- **Board v1 – I2C Interface Board**  
  Handles analog acquisition, user interface, and digital I/O expansion.

- **Board v2 – Main Controller Board**  
  Implements the central processing unit based on an ESP32 module, including sensor interfacing, storage, and communication.

---

## System Architecture

The system follows a modular architecture with a clear separation between acquisition and processing:

- **Board v1** performs:
  - High-resolution analog acquisition (ADS1115)
  - User interface (LCD + push buttons)
  - Digital I/O expansion (TCA9555)
  - Timekeeping (DS3231 RTC)

- **Board v2** performs:
  - Sensor acquisition (digital + optional analog)
  - Data logging (microSD)
  - Communication (WiFi / MQTT)
  - System coordination

Both boards are interconnected through an **I2C bus** and a dedicated connector.

---

## Functional Blocks

### Analog Acquisition (Board v1)
- External ADC: **ADS1115 (16-bit)**
- Channels:
  - pH
  - Electrical Conductivity (EC)
  - Dissolved Oxygen (O₂)
  - Auxiliary analog input
- Designed to overcome ESP32 ADC limitations

### Digital Expansion (Board v1)
- **TCA9555 (I2C I/O expander)**
- Manages:
  - Status LEDs (SD, WiFi, MQTT, Data)
  - Push buttons (Mode, OK, Up, Down)
  - Float switches
  - Relay outputs
  - Auxiliary digital I/O

### Real-Time Clock
- **DS3231 RTC**
- Provides timestamps for logging and system operation

### User Interface
- 20x4 I2C LCD display
- Push-button interface for local interaction

---

### Main Controller (Board v2)
- ESP32-based module (DFRobot FireBeetle)
- Handles:
  - Sensor acquisition
  - Data processing
  - Communication
  - Storage

---

### Sensors

#### Analog Sensors
- Connected via Board v1 (ADS1115)
- Require:
  - Signal conditioning
  - Galvanic isolation

#### Digital Sensors
- DHT22 (air temperature and humidity)
- DS18B20 (water temperature, 1-Wire)

---

### Storage
- microSD module (SPI interface)
- Used for:
  - Data logging
  - Configuration file (`config.txt`)

---

### Communication
- Current: WiFi + MQTT
- Future: Cellular communication (SIM module)

---

## Interconnections

- I2C bus shared across:
  - Board v1 and Board v2
  - LCD, ADC, RTC, and I/O expander

- Dedicated connector between boards:
  - SDA / SCL
  - Power (VCC / GND)
  - Interrupt line (INT)

- Analog signals routed through external conditioning modules

---

## Design Considerations

- **ESP32 ADC nonlinearity** → use of external ADC (ADS1115)
- **Analog isolation required** for water-based sensors
- **I2C bus centralization** for modular expansion
- **Interrupt-based signaling (TCA9555 → ESP32)** for efficient event handling
- **Multiple microSD wiring options** supported for flexibility

---

## Files

- `board_v1_schematic.pdf` → I2C interface board
- `board_v2_schematic.pdf` → main controller board

---

## Notes

- The system was initially developed for the Acuacol project but is designed as a reusable platform.
- Future versions will include improved ADC performance and cellular connectivity.


