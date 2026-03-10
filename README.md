# Road Maintenance Management System (RMMS) 
### IoT-Based Road Anomaly Detection with Geospatial Mapping and Image Evidence



## 📌 Project Overview
The **Road Maintenance Management System (RMMS)** is an integrated IoT solution designed to automate the identification and logging of road defects such as potholes and cracks. Using an **ESP32-CAM** as the edge-processing unit, the system utilizes a 6-axis Inertial Measurement Unit (IMU) to detect physical anomalies through vibration analysis. 

When a road defect is detected, the system triggers a **Signal-Trigger-Response (STR)** framework: it captures a high-resolution photo of the road, fetches precise GPS coordinates, and calculates vehicle tilt. All data is then logged locally to a CSV database and an image repository for road maintenance authorities.

## 🚀 Features
* **Automated Pothole Detection:** Real-time Z-axis acceleration monitoring using MPU6500.
* **Geospatial Logging:** Accurate Latitude and Longitude tagging via NEO-6M GPS.
* **Visual Evidence:** Automated JPEG capture via OV2640 camera module upon impact.
* **Tilt Analysis:** Calculation of Pitch and Roll to determine anomaly severity.
* **Local Data Management:** Python-based backend for automatic Excel-compatible CSV generation.

## 🛠 Hardware Architecture
The system is built using the following components:
* **Microcontroller:** ESP32-CAM (AI-Thinker)
* **Motion Sensor:** MPU6500 (6-Axis Accelerometer & Gyroscope)
* **Positioning Module:** NEO-6M GPS
* **Communication:** FTDI USB-to-TTL Serial Converter



## 📂 System Layers (IoT Architecture)
1.  **Perception Layer:** Hardware sensors (IMU, GPS, Camera) acquiring raw environmental data.
2.  **Network Layer:** Serial (UART) data transmission of sensor strings and binary image packets to a local host.
3.  **Application Layer:** Python script for data parsing, folder organization, and CSV logging.

## 💻 Installation & Setup

### 1. Arduino Firmware
1.  Install the `MPU6050` and `TinyGPS++` libraries in the Arduino IDE.
2.  Connect the ESP32-CAM to your PC using the FTDI adapter (Jumper **GPIO 0 to GND** for flashing).
3.  Upload the provided `.ino` sketch.
4.  **Important:** Remove the GPIO 0 jumper and press the **Reset** button after uploading.

### 2. Python Backend
1.  Navigate to your project directory:
    ```bash
    cd Desktop/MPU6500
    ```
2.  Install dependencies:
    ```bash
    pip install pyserial
    ```
3.  Run the logger:
    ```bash
    python rmms_logger.py
    ```

## 📊 Data Output
The system generates a `road_report.csv` file within the `MPU6500` folder. This file is directly compatible with **Microsoft Excel**.

| Timestamp | Latitude | Longitude | Accel_Z | Tilt_X | Tilt_Y | Image_Filename |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| Tue Mar 10 22:30:00 | 14.6507 | 120.9790 | 1.85 | 2.14 | -0.45 | pothole_20260310.jpg |



## 📜 Research Context
This project was developed as a prototype for automated road infrastructure monitoring. It addresses the limitations of manual road inspections by providing a low-cost, high-accuracy, and scalable alternative for local government units (LGUs).

## 👥 Contributors
* **Moraca, Angelica Mae I.**
* **Gonzales, Reginald Christian V.**
* **Lumibao, Czer Dominic**
* **Lopez, Lorenzo Paul F.**
