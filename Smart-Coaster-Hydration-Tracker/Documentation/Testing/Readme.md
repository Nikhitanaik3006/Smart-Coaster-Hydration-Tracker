# Testing and Validation

The Testing and Validation section contains the results obtained from testing the Smart Coaster prototype.

The prototype was physically tested to verify the bottle-weight measurement, water-level estimation, sip detection, and Bluetooth communication functionality.

---

## 1. Serial Output Testing

The ESP32 serial output was observed during system operation to verify the measurements and processing performed by the firmware.

The serial output provides information related to the measured bottle weight and hydration monitoring process.

**Test Result:** `01_Serial_Test_Output.png`

---

## 2. BLE Testing using nRF Connect

Bluetooth Low Energy communication was tested using the **nRF Connect** mobile application.

The ESP32 was connected to the mobile device through BLE, and the transmitted hydration data was observed using the application.

This test verifies the wireless communication between the ESP32 and the mobile device.

**Test Result:** `02_BLE_Testing_nRF_Connect.png`

---

## 3. Weight Measurement Validation

The weight measurement of the Smart Coaster was validated by comparing the measured values with a reference digital weighing machine.

A water bottle was placed on the Smart Coaster, and the measured weight was compared with the reference measurement.

**Test Result:** `03_Weight_Measurement_Validation.png`

---

## 4. Weight Measurement Validation Report

The complete weight measurement validation and prototype testing results are documented in the validation report.

The report contains the testing setup, reference measurements, Smart Coaster measurements, and observed results.

**Validation Report:** `04_Weight_Measurement_Validation.pdf`

---

## Testing Summary

The prototype testing covered the following functions:

- **Bottle-weight measurement**
- **Weight measurement validation**
- **Water-level estimation**
- **Sip detection**
- **BLE communication**
- **Mobile monitoring using nRF Connect**

The testing results provide validation of the implemented Smart Coaster prototype and its major functional components.
