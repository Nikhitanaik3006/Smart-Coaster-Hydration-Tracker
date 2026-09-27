# System Architecture

The Architecture section describes the internal working flow of the Smart Coaster for Hydration Monitoring system, including the sensing interface, data processing, communication, and hydration monitoring algorithms.

---

## 1. Complete System Architecture

The complete architecture shows the overall flow of data through the Smart Coaster system.

The **load cell** senses the bottle weight. The **HX711** interfaces the load cell with the **ESP32**. The ESP32 collects and processes the measurement data, calculates the bottle weight and remaining water level, detects water intake, and communicates the hydration information through **BLE**.

**Diagram:** `Complete_Architecture.png`

---

## 2. Driver Interface

### ESP32–HX711 Communication

The HX711 acts as the interface between the load cell and the ESP32.

The ESP32 communicates with the HX711 using the **DT (Data) and SCK (Clock)** signals to obtain the load-cell measurement.

**Diagram:** `Drivers/01_ESP32_HX711_Communication.png`

---

## 3. Communication

### ESP32–BLE Communication

The ESP32 provides wireless communication between the Smart Coaster and the mobile device.

The hydration data processed by the ESP32 is transmitted through **Bluetooth Low Energy (BLE)**. The mobile device can monitor the transmitted information using the **nRF Connect** application.

The BLE interface also supports commands such as **TARE** and **RESET**.

**Diagram:** `Communication/01_ESP32_BLE_Communication.png`

---

##S 4. ProcessSing Algorithms

The Smart Coaster firmware uses multiple processing stages to obtain stable weight measurements and determine hydration-related information.

### 4.1 Sample Collection

The system collects load-cell measurements over a defined number of samples before further processing.

**Algorithm:** `Algorithms/01_Sample_collection.png`

---

### 4.2 Buffer Processing

The collected samples are processed to obtain a stable measurement and reduce the effect of measurement noise.

**Algorithm:** `Algorithms/02_Buffer_Processing.png`

---

### 4.3 Weight and Water-Level Calculation

The processed measurement is used to determine the bottle weight.

The system uses the measured bottle weight to estimate the remaining water level as a percentage.

**Algorithm:** `Algorithms/03_Weight & Water Level Calculation.png`

---

### 4.4 Bluetooth Communication

The processed hydration information is communicated from the ESP32 to the mobile device through BLE.

This stage handles the transmission of the measured information and BLE communication with the mobile application.

**Algorithm:** `Algorithms/04_Bluetooth_Communication.png`

---

### 4.5 Sip Detection

Sip detection is based on changes in the measured bottle weight.

When the change in weight crosses the defined threshold, the system identifies water intake and updates the sip count.

**Algorithm:** `Algorithms/05_Sip_Detection.png`

---

## 5. Overall Data Flow

The overall processing flow of the Smart Coaster can be summarized as:

**Load Cell → HX711 → Sample Collection → Buffer Processing → Weight & Water-Level Calculation → Sip Detection → BLE Communication → Mobile Device**

The ESP32 performs the measurement processing and communicates the resulting hydration information to the mobile device through BLE.
