# Study-Group-IoT

Repository for the IEEE Computer Society study group on IoT at the University of Brasília (UnB). It collects the programs developed during the meetings, in which sensors and LEDs connected to an **ESP32** board are read and controlled remotely, either through the **Blynk** app or through a web page served by the ESP32 itself.

## Structure

All the code is in the [`Programas/`](Programas/) folder. Each subfolder is an Arduino IDE sketch (`.ino`).

| Folder | What it does |
|---|---|
| `Blink/` | Basic test: blinks an LED (pin 2) every 500 ms. |
| `Luz/` | Reads a light sensor (LDR), sends the value to Blynk and turns on an alert LED. A button in the app inverts the logic (LED on with light or in the dark). |
| `Movimento/` | Reads a PIR motion sensor (HC-SR501), shows its state in Blynk and drives an LED. A button in the app inverts the logic. |
| `movimentoControleFrequenciaPeloBlynk/` | Evolution of `Movimento/`: how long the LED stays on after motion is detected is set with a slider in Blynk. |
| `Temperatura_e_Umidade/` | Reads temperature and humidity with a DHT11 sensor, sends them to Blynk and turns on an alert LED when values cross the configured limits. |
| `TemperaturaEUmidade/sketch_oct28a/` | Early version of the temperature and humidity sketch, without the alert LED. It only sends readings to Blynk every second. |
| `Semaforo/Blynk/Semaforo/` | Traffic light (red, yellow and green LEDs) controlled by buttons in Blynk. |
| `Semaforo/ServidorNoESP32/` | Same traffic light, but controlled through a web page hosted on the ESP32 and accessed over the local Wi-Fi network, with no dependency on Blynk. |
| `Unificação dos Sensores/UnifiedCode/` | Integrated "Casa Automática" (automated home) project: combines the TCRT5000 sensor (motion), the LDR (light) and the DHT11 (temperature and humidity) in a single sketch, with pins rearranged to avoid conflicts. |
| `Unificação dos Sensores/sketch_may19a/` | Variant of the integrated project, with separate LEDs for temperature and humidity. |

The file `Programas/config_wifi.h` exists but is currently empty.

## Hardware

- ESP32 board
- Light sensor (LDR)
- PIR motion sensor HC-SR501
- TCRT5000 reflective infrared sensor
- DHT11 temperature and humidity sensor
- LEDs and resistors
- Breadboard and jumper wires

## Software and libraries

- [Arduino IDE](https://www.arduino.cc/en/software) with ESP32 board support
- **Blynk** library (`BlynkSimpleEsp32.h`)
- **DHT sensor library** (`DHT.h`), for the sketches that use the DHT11
- `WiFi.h` and `WebServer.h`, which ship with the ESP32 board package

## How to use

1. Install the Arduino IDE, ESP32 support and the libraries above.
2. Open the `.ino` file of the project you want.
3. Fill in your Wi-Fi credentials (`ssid` and `pass`) at the top of the sketch.
4. For the Blynk projects, create a template in the [Blynk Console](https://blynk.cloud), set up the virtual pins used in the code, and fill in `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME` and `BLYNK_AUTH_TOKEN` with your device's values.
5. Wire the circuit according to the pins defined in the sketch, select the board and port, and upload the code.
6. For the `ServidorNoESP32` project, open the serial monitor to see the IP address and visit it in a browser on the same network.

## Security notice

Sketches should not be published with real credentials. Before committing, keep `ssid`, `pass` and `BLYNK_AUTH_TOKEN` as placeholders, or move them to a local file such as `config_wifi.h` listed in `.gitignore`.

## Context

Project of the IEEE Computer Society IoT study group, UnB chapter.
