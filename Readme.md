# SmartBin - IoT-Based Waste Management System

## Objective
The goal of this project was to design and develop a **smart waste management system** using **ESP32 and IoT technology**. SmartBin detects **wet and dry waste**, monitors bin levels using **ultrasonic sensors**, and segregates waste automatically with a **servo motor**. Data is sent to a **Flutter-based mobile app** and **Firebase**, providing real-time monitoring and notifications. This project combines embedded systems, IoT, and cloud integration to improve waste collection efficiency.

---

### Skills Learned

- Programming ESP32 for **sensor integration and IoT applications**.  
- Implementing **non-contact waste classification** using a rainwater/soil moisture sensor.  
- Controlling **servo motors** for waste segregation.  
- Monitoring **bin levels** with ultrasonic sensors.
- Reading and processing **GPS data** from the NEO-6M module 
- Integrating **Firebase Realtime Database** for long-term data storage.  
- Debugging IoT systems combining multiple sensors and cloud communication.  

---

### Tools Used

- **ESP32** — main controller for sensors, motor, and cloud integration.  
- **Ultrasonic Sensors (3)** — front (incoming waste), wet bin, dry bin.  
- **Rainwater/Soil Moisture Sensor** — classifies wet or dry waste.  
- **SG90 Servo Motor** — actuates waste segregation mechanism.
- **NEO-6M GPS module** — provides latitude and longitude coordinates.  
- **Firebase Realtime Database** — stores key messages like bin status.  
- **Flutter Mobile App** — real-time monitoring of SmartBin.  
- **Arduino IDE** — programming and uploading code.  

---

## Steps

### 1. Hardware Setup
The SmartBin hardware was assembled with:  
- Three **ultrasonic sensors** for waste detection and bin-level monitoring  
- **Rainwater/soil moisture sensor** for waste classification  
- **Servo motor** for waste segregation
- **NEO-6M GPS module** — provides latitude and longitude coordinates. 
- **ESP32 controller** for processing and cloud integration  

The system continuously monitors incoming waste. If a bin reaches a critical level (less than 15 cm free space), the servo will **not accept more waste**, and the app receives a **notification**.

![Hardware Setup](https://github.com/aswinR-0210/SmartBIn/blob/main/circuit.jpg)

---

### 2. Waste Detection and Classification
- **Ultrasonic Sensor 1** detects incoming waste.  
- **Rainwater sensor** identifies whether waste is wet or dry.  
- Servo motor moves the waste to the appropriate bin based on classification:  
  - **Wet waste → wet bin**  
  - **Dry waste → dry bin**  

![Waste Detection](link-to-your-detection-image)

---

### 3. Bin Level Monitoring
- **Ultrasonic Sensor 2** monitors the wet waste bin level.  
- **Ultrasonic Sensor 3** monitors the dry waste bin level.  
- If the distance to waste surface is less than **15 cm**, the system stops accepting waste and sends a **notification to the Flutter app**.  
- Once space is available again, the system resumes operation.

![Bin Level Monitoring](link-to-your-bin-image)

---
### 4.GPS Location Tracking
- The **NEO-6M GPS module** connected to ESP32 sends **latitude and longitude** readings.  
- The location is updated to Firebase **every 1 hour**.  
- This allows tracking each SmartBin’s location through the **Flutter app**.  

---

### 5. Cloud Integration
- **Firebase Realtime Database** stores:
  - Wet/Dry bin levels  
  - Waste type  
  - GPS location and timestamps  
- **HTTP communication** is used between ESP32 and Firebase.  
- The **Flutter mobile app** reads data in real-time and shows:
  - Bin level percentage  
  - Bin full alerts  
  - Last updated GPS location 


---

### 6. Servo Motor Operation
- Servo rotates left or right depending on the **waste type** detected.  
- After waste is deposited, the servo returns to the **center position** ready for the next operation.  

---

### 7. Notifications and App Monitoring
- **Bin full notifications** are sent to the Flutter app when either wet or dry bin is full.  
- **Real-time data updates** allow users to monitor bin levels remotely.
- Last known location of the bin. 

---

### 8. Conclusion
SmartBin demonstrates a **complete IoT waste management solution** combining sensors, cloud integration, and automation. It improves efficiency in waste segregation and collection, while reinforcing skills in **embedded systems, IoT communication, cloud databases, and mobile app integration**.
