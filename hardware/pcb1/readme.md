# Board v1 – I2C Interface Board

## Description
This board implements the I2C-based interface layer of the IoT aquaculture device. It integrates user interaction, timing, and analog data acquisition functionalities, acting as a peripheral expansion module for the main controller.

## Features

- **I2C LCD Display (20x4)**  
  Provides user interface for system status, sensor readings, and calibration procedures.

- **ADS1115 Analog-to-Digital Converter (16-bit)**  
  Connected to four analog channels for:
  - Temperature (auxiliary analog input)
  - pH measurement
  - Electrical conductivity (EC)
  - Dissolved oxygen (O₂)  
  Enables high-resolution acquisition of low-level analog signals.

- **DS3231 Real-Time Clock (RTC)**  
  Provides accurate timekeeping for:
  - Data logging
  - Timestamping measurements  
  Includes battery backup for operation during power loss.

- **TCA9555 I/O Expander (16-bit, I2C)**  
  Manages multiple digital inputs and outputs:
  - 4 status LEDs
  - 4 push buttons (user interface)
  - 2 float switches (level detection)
  - 2 auxiliary digital inputs
  - 2 relay control outputs
  - 2 auxiliary digital outputs  

  Reduces GPIO usage on the main microcontroller and centralizes digital I/O management.

## Notes

- Originally designed for the Acuacol project, but intended as a reusable module within a broader monitoring platform.

- Analog sensors (pH, EC, O₂) require **proper signal conditioning and galvanic isolation**, which is implemented externally through dedicated analog front-end modules.

- Communication with the main controller is exclusively performed via the I2C bus.
