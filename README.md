# Elegoo-Smart-Robot-Smooth-ControlV3_0
This is the source code to run the Elegookit iPhone app that was developed for the Robot car 3 Plus for the old Robot Car V3.0 I have reverse engineered the commands of the app and now you can still use your old Robot Car Kit 3.0 with the new app. 

# Elegoo Smart Robot Car - Smooth App & Line Control

This repository contains an optimized Arduino sketch for the **Elegoo Smart Robot Car (V3.0/V4.0)**. It replaces the jerky standard movements with smooth, professional logic and unlocks the full potential of the Bluetooth control app.

## 🚀 Key Features

* **Dynamic Speed Control:** Real-time speed adjustment (0-255 PWM) using the 10-bar slider in the app (via `D2` data parsing).
* **8-Way Precision Steering:** Fully mapped joystick control, including all diagonal movements (Forward-Left/Right, Back-Left/Right).
* **Smooth Line Tracking:** Uses a differential steering algorithm specifically tuned for **4cm wide tape**. Instead of "zig-zagging," the car performs smooth arc turns.
* **Autonomous Obstacle Avoidance:** Integrated "Radar Scan" – the car stops, looks left and right with the ultrasonic sensor, and chooses the path with the most clearance.
* **Mode Switching:** Seamlessly switch between Manual Bluetooth control, Line Tracking, and Obstacle Avoidance via app commands.

## 🛠 Hardware Configuration

| Component | Arduino Pins |
| :--- | :--- |
| **Motor Driver (L298N)** | ENA: 5, ENB: 6, IN1: 7, IN2: 8, IN3: 9, IN4: 11 |
| **Line Sensors** | Left: 2, Middle: 4, Right: 10 |
| **Ultrasonic Sensor** | Trig: A5, Echo: A4 |
| **Servo Motor** | Pin 3 |

## 📱 App Mapping

This code is designed to work with the standard Bluetooth Sniffer/Controller apps. The protocol uses the following data structure:
* **D1:** Directional commands (0-9).
* **D2:** Speed/Throttle values (0-255).

| Command (D1) | Action |
| :--- | :--- |
| `3` | Forward |
| `4` | Backward |
| `6` / `8` | Forward Diagonals |
| `7` / `9` | Backward Diagonals |
| `1` / `2` | Spin Left / Right |
| `0` | Stop |

## 🔧 Installation

1. Download the `RobotControl.ino` file from this repo.
2. Open it in the **Arduino IDE**.
3. Ensure you have the `Servo` library installed.
4. Connect your Arduino Uno via USB and upload the code.
5. Connect via Bluetooth and start driving!

## 📜 License
This project is open-source and available under the **MIT License**.
