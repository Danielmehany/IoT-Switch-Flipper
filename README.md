# IoT Switch Flipper

**Turn off the lights from bed!**

An ESP32-powered device that lets you flip a light switch using a servo motor, controlled from your phone through a local web app.

<p align="center">
  <img src="https://github.com/user-attachments/assets/2bc33a1b-c66c-4278-b1d3-77427f4f5b31" width="400" />
</p>

---

## PCB Design

The PCB was designed in Altium Designer and includes an ESP32 MCU, 3-pin servo header, dual AA battery holders, 3V → 5V boost converter, 5V → 3.3V LDO, USB-C port, and USB-UART converter.

### Power Architecture

```
AA Batteries (3V) → Boost Converter (5V) → LDO (3.3V) → ESP32
                            ↓
                     Servo Motor (5V)
```
---

<p align="center">
  <img src="https://github.com/user-attachments/assets/78e2f2f8-3ad3-41fc-bebf-53982bf5b481" width="48%" />
  <img alt="image" src="https://github.com/user-attachments/assets/a090141e-8eae-4d55-a71a-377f6ef6f959" width="48%" />
</p>

---

## Software

The ESP32 is programmed in C++ using the Arduino IDE. It hosts a local web server that serves an HTML page with an interactive toggle switch. When the switch is toggled in the browser, the ESP32 sends a PWM signal to rotate the servo motor and flip the physical light switch.

### How It Works

1. ESP32 connects to your local Wi-Fi network
2. Access the web app by navigating to the ESP32's IP address
3. Toggle the switch in the app
4. ESP32 receives the command and sends PWM signal to servo
5. Servo rotates to flip the physical light switch

<p align="center">
  <img src="https://github.com/user-attachments/assets/97140d54-73ef-434a-96db-2d74fe2c82eb" width="48%"/>
  <img src="https://github.com/user-attachments/assets/7c80ea3d-e007-4a64-9513-bd756003bf5c" width="48%"/>
</p>
