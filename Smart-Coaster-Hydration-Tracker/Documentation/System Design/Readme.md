# System Design

The System Design section describes the hardware architecture and electrical design of the Smart Coaster for Hydration Monitoring system.

## 1. System Block Diagram

The system block diagram presents the overall functional flow of the Smart Coaster.

The **load cell** senses the weight of the water bottle. The **HX711 load-cell amplifier** interfaces the load cell with the **ESP32**. The ESP32 processes the measured data, detects changes in bottle weight, calculates the remaining water level, and detects sips.

The processed hydration information is transmitted to a mobile device through **Bluetooth Low Energy (BLE)**, where it can be monitored using the **nRF Connect application**.

**Diagram:** `01_system-block-diagram.png`

---

## 2. Schematic

The schematic shows the electrical connections between the major hardware components of the Smart Coaster.

It includes the connections between the **ESP32, HX711, and load cell**, along with the relevant power and signal connections.

**Diagram:** `02_Schematic.png`

---

## 3. ERC Verification

The Electrical Rules Check (ERC) is used to verify the electrical connections in the schematic.

The ERC result helps identify potential electrical connection and design-rule issues in the circuit before hardware implementation.

**Diagram:** `03_ERC_Verification.png`

---

## System Design Summary

The Smart Coaster follows the flow:

**Load Cell → HX711 → ESP32 → BLE → Mobile Device**

The load cell provides the sensing function, HX711 provides the load-cell interface, ESP32 performs measurement processing and hydration calculations, and BLE provides wireless communication with the mobile device.
