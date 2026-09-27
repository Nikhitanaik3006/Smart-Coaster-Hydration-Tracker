# Smart Coaster Prototype

The Prototype section contains photographs of the physically implemented Smart Coaster for Hydration Monitoring system.

The prototype demonstrates the integration of the sensing, processing, and communication components used in the project.

---

## 1. Physical Prototype

The Smart Coaster prototype consists of a coaster platform integrated with a **load cell** for measuring the weight of a water bottle.

The load-cell measurement is interfaced with the **HX711 load-cell amplifier**, which provides the measurement data to the **ESP32**.

The ESP32 processes the measured data and communicates the hydration information to a mobile device using **Bluetooth Low Energy (BLE)**.

**Prototype Image:** `01_Smart_Coaster_Prototype.jpg`

---

## 2. Prototype Components

The implemented prototype includes:

- **ESP32** – Main processing and BLE communication
- **Load Cell** – Measures the bottle weight
- **HX711** – Interfaces the load cell with the ESP32
- **Coaster Platform** – Supports the water bottle and load-cell assembly
- **Power and Signal Connections** – Provide electrical connections between the components

---

## 3. Prototype Working Flow

The physical prototype follows the measurement flow:

**Water Bottle → Load Cell → HX711 → ESP32 → BLE → Mobile Device**

The load cell detects changes in bottle weight. The HX711 provides the load-cell measurement to the ESP32, where the data is processed for hydration monitoring.

The resulting information can then be transmitted to a mobile device through BLE.

---

## 4. Prototype Implementation

The photograph demonstrates the physical implementation and integration of the major hardware components used in the Smart Coaster system.

The prototype was subsequently tested to verify weight measurement, hydration monitoring, and BLE communication functionality. Detailed test results are available in the **Testing** section.
