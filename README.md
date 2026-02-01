# Elegoo Smart Robot Car V3.0 (Original) – Smooth Control Edition 🤖🚀

This project features a highly optimized firmware specifically developed for the **Elegoo Smart Robot Car Kit V3.0 (Original)**. Unlike the standard factory code, this version focuses on **smooth motion profiles**, **non-blocking sensor logic**, and **refined navigation algorithms**.

---

## 📺 Project Demo
<div align="center">
  <a href="https://www.youtube.com/watch?v=iPJJ6ZrjQEc">
    <img src="https://img.youtube.com/vi/iPJJ6ZrjQEc/maxresdefault.jpg" alt="Watch the Demo" style="width:100%; max-width:600px; border-radius: 10px; box-shadow: 0px 4px 15px rgba(0,0,0,0.3);">
  </a>
  <p><em>Click the image above to watch the robot in action!</em></p>
</div>

---

## ⚠️ Vital Pre-Installation Notes

### 1. Version & Compatibility
**This code was developed and tested on the Original V3.0 version of the Elegoo Robot Car.**
* **V3.0 Plus Users:** While the pinouts are identical to the Plus version, this firmware has **not been tested** on the Plus hardware. It *should* work theoretically, but use it at your own risk.
* **V4.0/Other Users:** Not compatible without significant pin re-assignment.

### 2. Bluetooth Module Warning
**ALWAYS REMOVE THE BLUETOOTH MODULE FROM THE ROBOT BEFORE UPLOADING THE CODE VIA USB!** The Bluetooth module occupies the Serial communication lines (TX/RX). If it remains plugged in during the upload, the Arduino IDE will fail to communicate with the board.

---

## ✨ Key Improvements over Stock Firmware

While the original Elegoo example code provides basic functionality, this **Smooth Control** version introduces several technical upgrades:

| Feature | Standard Elegoo Code | Smooth Control V3.0 |
| :--- | :--- | :--- |
| **Movement** | Digital "On/Off" (Jerkiness) | Adaptive PWM Ramping (Smooth Start/Stop) |
| **Steering** | Pivot turns only (Fixed Speed) | Differential Drive for fluid, natural curves |
| **Logic** | Blocking `delay()` calls | Non-blocking `millis()` architecture |
| **Responsiveness** | Limited during sensor sweeps | Parallel tasking (polling IR/BT during moves) |

---

## 🛠 Features
* **Smooth Obstacle Avoidance:** The robot scans its environment while moving and calculates the optimal path without halting the CPU.
* **Precision Line Tracking:** Uses refined logic to stay on track even at higher speeds, significantly reducing the "zig-zag" effect.
* **Hybrid Control:** Seamlessly switch between IR Remote, Bluetooth, and Autonomous modes.

---

## 🔌 Hardware Configuration (V3.0 Original)
To ensure the code works out of the box, verify your wiring against these pins:
* **Motors:** ENA(5), ENB(6), IN1(7), IN2(8), IN3(9), IN4(11)
* **Ultrasonic:** Trig(A5), Echo(A4)
* **IR Receiver:** Pin 12
* **Line Sensors:** Left(2), Middle(4), Right(10)
* **Servo:** Pin 3

---

## 🚀 Installation
1.  **Clone the repo:**
    ```bash
    git clone [https://github.com/TheTechNerd777/Elegoo-Smart-Robot-Smooth-ControlV3_0.git](https://github.com/TheTechNerd777/Elegoo-Smart-Robot-Smooth-ControlV3_0.git)
    ```
2.  **Required Libraries:** Ensure you have `IRremote` and `Servo` installed in your Arduino IDE.
3.  **Prepare for Upload:** **Disconnect the Bluetooth module.**
4.  **Upload:** Open the `.ino` file, select **Arduino Uno**, and upload.
5.  **Reconnect:** Plug the Bluetooth module back in.

---

## 📜 License
Distributed under the MIT License. See `LICENSE` for more information.

---
<p align="center">Made with ❤️ by <a href="https://github.com/TheTechNerd777">TheTechNerd777</a></p>
