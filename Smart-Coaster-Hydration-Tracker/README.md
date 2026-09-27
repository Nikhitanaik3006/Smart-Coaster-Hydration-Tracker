# Smart Coaster for Hydration Monitoring

**An IoT-based hydration monitoring system using ESP32, a load cell, HX711, and Bluetooth Low Energy (BLE).**

---

## Project Overview

The **Smart Coaster** is an IoT-based hydration monitoring system developed using an **ESP32** and a **load cell** to monitor water consumption in real time.

The system measures changes in the weight of a water bottle placed on the coaster. These weight changes are processed to detect water intake, estimate the remaining water level, and track the number of sips. The hydration data is transmitted wirelessly to a mobile device using Bluetooth.

---

## Key Features

- **Real-time bottle weight measurement**
- **Detection of water intake based on changes in bottle weight**
- **Estimation of remaining water percentage (0–100%)**
- **Real-time sip detection and sip count**
- **Bluetooth Low Energy (BLE) communication**
- **Mobile monitoring using the nRF Connect application**
- **ESP32-based embedded firmware**

---

## System Components

### Hardware

- **ESP32**
- **Load Cell**
- **HX711 Load Cell Amplifier**
- **Water Bottle**
- **Power Supply**

### Software

- **ESP-IDF**
- **C Programming**
- **Bluetooth Low Energy (BLE)**
- **nRF Connect Mobile Application**

---

## Working Principle

1. The water bottle is placed on the Smart Coaster.
2. The load cell measures the weight of the bottle.
3. The HX711 interface provides the load-cell measurement to the ESP32.
4. The ESP32 processes the measured weight and filters measurement noise.
5. Changes in bottle weight are used to detect water consumption.
6. The remaining water level is calculated as a percentage.
7. A sip is detected when the change in bottle weight crosses the defined threshold.
8. The hydration information is transmitted to a mobile device through Bluetooth.
9. The transmitted data can be viewed using the nRF Connect application.

---

## Firmware

The firmware is developed for the **ESP32** using the **ESP-IDF framework**.

The firmware includes:

- **Load-cell data acquisition**
- **Weight measurement and processing**
- **Noise filtering and stability detection**
- **Bottle water-level calculation**
- **Sip detection**
- **BLE communication**
- **BLE data notification**
- **BLE command handling**

The firmware uses BLE services and characteristics to communicate the measured hydration information with a mobile device.

---

## BLE Communication

The ESP32 operates as a BLE device and advertises itself as:

`ESP32_WEIGHT`

The mobile device can connect to the ESP32 using the **nRF Connect** application.

The firmware supports BLE data communication and commands such as:

- `TARE` – performs tare/zeroing of the weight measurement
- `RESET` – resets the relevant measurement state

---

## Testing and Validation

The Smart Coaster prototype was physically tested to validate the **bottle-weight measurement** and **hydration monitoring functionality**.

Testing was carried out using a water bottle placed on the load cell, and the measured weight values were observed through the ESP32 system.

The testing process included:

- **Testing the load-cell-based bottle weight measurement**
- **Verifying weight measurements for different bottle conditions**
- **Observing changes in measured weight during water consumption**
- **Verifying water-level estimation based on bottle weight**
- **Testing sip detection based on changes in bottle weight**
- **Verifying Bluetooth communication between the ESP32 and mobile device**
- **Monitoring the transmitted data using the nRF Connect application**

The test results and prototype testing setup are included in the project documentation.

---

## Project Structure

```text
Smart-Coaster-Hydration-Tracker/
│
├── README.md
├── .gitignore
│
├── firmware/
│   ├── CMakeLists.txt
│   ├── sdkconfig
│   │
│   └── main/
│       ├── CMakeLists.txt
│       └── hello_world_main.c
│
└── Documentation/
    │
    ├── Architecture/
    │   ├── Algorithms/
    │   │   ├── 01_Sample_collection.png
    │   │   ├── 02_Buffer_Processing.png
    │   │   ├── 03_Weight & Water Level Calculation.png
    │   │   ├── 04_Bluetooth_Communication.png
    │   │   └── 05_Sip_Detection.png
    │   │
    │   ├── Communication/
    │   │   └── 01_ESP32_BLE_Communication.png
    │   │
    │   ├── Drivers/
    │   │   └── 01_ESP32_HX711_Communication.png
    │   │
    │   └── Complete_Architecture.png
    │
    ├── System Design/
    │   ├── 01_system-block-diagram.png
    │   ├── 02_Schematic.png
    │   └── 03_ERC_Verification.png
    │
    ├── Testing/
    │   ├── 01_Serial_Test_Output.png
    │   ├── 02_BLE_Testing_nRF_Connect.png
    │   ├── 03_Weight_Measurement_Validation.png
    │   └── 04_Weight_Measurement_Validation.pdf
    │
    └── Smart-Coaster-Hydration-Tracker-Presentation.pdf
