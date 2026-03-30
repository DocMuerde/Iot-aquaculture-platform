# IoT Aquaculture Platform

## Overview
This repository contains a modular IoT platform designed for environmental monitoring and control in aquaculture systems.

The platform integrates sensing, data logging, and communication capabilities, and is intended to be reusable across multiple projects (e.g., Acuacol, Acuafasa, and future developments).

## System Description
The system is based on an embedded device (ESP32-based) capable of:

- Monitoring environmental and water parameters:
  - Air temperature and humidity (DHT22)
  - Water temperature (DS18B20)
  - pH, conductivity, and dissolved oxygen (analog sensors)
- Logging data locally on a microSD card
- Sending data to a remote server via MQTT over WiFi
- Supporting future control actions (e.g., pumps, valves)

## Repository Structure

- `firmware/` → Embedded software (Arduino/ESP32)
- `docs/manuals/` → User manuals and operation guides
- `docs/technical/` → Technical documentation and system architecture
- `docs/notes/` → Development notes and design decisions
- `hardware/pcb/` → PCB designs (EasyEDA exports, Gerbers)
- `hardware/schematics/` → Electrical schematics
- `redesign/` → Future versions and system redesigns
- `images/` → Images used in documentation

## Current Implementation
The current version of the system has been developed in the context of the Acuacol project.

It includes:
- Multi-sensor acquisition
- Data logging in microSD
- WiFi connectivity and MQTT communication
- Sensor calibration via onboard interface

See the user manual:
- `docs/manuals/user-manual-acuacol-v1.pdf`

## Hardware
The hardware consists of a custom-designed PCB integrating:
- ESP32 microcontroller
- Analog front-end for sensors
- Isolated measurement modules
- Relay outputs (prepared for future use)

See:
- `hardware/pcb/`
- `hardware/schematics/`

## Firmware
The firmware is developed using Arduino IDE and ESP32 libraries.

It handles:
- Sensor acquisition
- Calibration routines
- Communication with server (MQTT)
- Data storage

See:
- `firmware/`

## Documentation
User and technical documentation can be found in:

- `docs/manuals/`
- `docs/technical/`

## Future Developments
A major redesign of the platform is planned, including:

- Integration of cellular connectivity (SIM module)
- Reduced dependency on WiFi infrastructure
- Improved robustness and deployment flexibility

See:
- `redesign/`

## Authors / Contributors
Ignacio Alvarado Aldea  
Department of Systems Engineering and Automation  
University of Seville

## License
To be defined.
