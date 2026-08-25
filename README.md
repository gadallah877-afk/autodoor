# Cloud-Integrated IoT Smart Door 🚪☁️

## Overview
A full-stack IoT automation platform built to control and monitor physical access securely. This project integrates an ESP32 microcontroller with a real-time Firebase backend, allowing users to automatically open and close a door via a responsive web application (Landing Page) from anywhere in the world.

## Tech Stack & Hardware 🛠️
* **Microcontroller:** ESP32
* **Actuators & Sensors:** L298N Motor Driver, DC Motor (for door mechanism)
* **Cloud Database:** Firebase Firestore / Realtime DB
* **Frontend:** HTML, JavaScript (Deployed on GitHub Pages)
* **Programming Languages:** C++ (Arduino IDE), JS

## Key Features ✨
* **Remote Web Control:** A dedicated, user-friendly landing page that communicates directly with Firebase to send open/close commands to the hardware instantly.
* **Real-Time Synchronization:** Achieves 99% network uptime, ensuring that the physical door responds immediately to the web app's button clicks without noticeable latency.
* **Automated Logic:** Efficiently processes daily sensor data and manages secure API requests for seamless data transmission between the UI and the ESP32.
* **Secure Data Management:** Implements robust database interactions to prevent unauthorized access while maintaining a high volume of requests.

## Project Demo 📸
<img width="1600" height="1200" alt="WhatsApp Image 2026-08-25 at 5 12 07 AM" src="https://github.com/user-attachments/assets/1680895c-094c-4e78-8406-c7d6c54709f6" />

## Live Website 🌐
[Click here to view and test the Web Application Interface](https://gadallah877-afk.github.io/autodoor/)
