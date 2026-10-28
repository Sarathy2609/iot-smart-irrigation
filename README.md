# IoT-Based Smart Irrigation and Soil Management System

A final-year engineering student prototype using an **ESP32**, capacitive soil-moisture sensor, relay/driver, and low-voltage DC pump. Optional DHT22 and tank-level switch support environmental monitoring and dry-run protection. Sensor readings can be sent to ThingSpeak when Wi-Fi is available; irrigation control continues locally if Wi-Fi/cloud access is lost.

> **Safety:** This is a student prototype, not a certified agricultural controller. Use only a correctly rated low-voltage DC pump and driver. Never connect a pump directly to an ESP32 GPIO. Keep mains wiring out of the prototype. Test with the pump disconnected first.

## Features
- Soil-moisture sampling and percentage mapping from calibrated ADC values.
- Hysteresis thresholds to avoid rapid relay switching.
- Pump maximum-run-time safety timeout.
- Optional tank-empty interlock.
- Optional DHT22 temperature/humidity monitoring.
- ThingSpeak telemetry (moisture, temperature, humidity, pump state, tank status).
- Local control remains active when Wi-Fi is unavailable.

## Repository layout
```text
iot-smart-irrigation-soil-management/
├── README.md
├── LICENSE
├── .gitignore
├── firmware/
│   ├── smart_irrigation.ino
│   └── platformio.ini
├── hardware/
│   ├── bill_of_materials.md
│   └── wiring.md
├── dashboard/
│   └── thingspeak_setup.md
├── docs/
│   ├── project_guide.md
│   └── testing_checklist.md
└── IoT_Smart_Irrigation_Project_Guide.pdf
```

## Hardware
Required:
- ESP32 development board
- Capacitive soil-moisture sensor with analog output
- DC pump rated for its supply
- Relay module or suitable MOSFET driver rated for the pump
- Separate suitable pump supply, jumper wires, tubing, and enclosure

Optional:
- DHT22 temperature/humidity sensor
- Float switch / tank-level sensor

See [`hardware/bill_of_materials.md`](hardware/bill_of_materials.md) and [`hardware/wiring.md`](hardware/wiring.md).

## Software
- Arduino IDE or PlatformIO
- ESP32 board support
- `DHT sensor library` by Adafruit (only if `USE_DHT` is enabled)
- ThingSpeak account/channel (optional)

## Quick start (Arduino IDE)
1. Install Arduino IDE and the Espressif ESP32 board package.
2. Install **DHT sensor library** by Adafruit if using the DHT22.
3. Open `firmware/smart_irrigation.ino`.
4. Set Wi-Fi credentials and ThingSpeak Write API key in the configuration section. Keep real credentials private; do not commit them to a public repository.
5. Verify `MOISTURE_DRY_RAW` and `MOISTURE_WET_RAW` using readings from your own sensor in dry and moist soil.
6. Check `RELAY_ACTIVE_LOW` and the relay board logic before connecting the pump.
7. Select your ESP32 board and port, upload, and open Serial Monitor at **115200 baud**.
8. Test with the pump disconnected. Confirm that dry soil requests watering, moist soil stops it, an empty tank prevents watering (if fitted), and the timeout stops the pump.
9. Configure ThingSpeak using [`dashboard/thingspeak_setup.md`](dashboard/thingspeak_setup.md).

## Calibration
ADC values differ between sensors, boards, supply voltages, and soil. The defaults in the sketch are examples only:
- `MOISTURE_DRY_RAW`: ADC value observed in dry soil.
- `MOISTURE_WET_RAW`: ADC value observed in thoroughly moist soil.

Record actual readings and adjust both constants. The sketch maps those values to 0–100%; validate the displayed percentage before using it for control. Set `MOISTURE_ON_PERCENT` and `MOISTURE_OFF_PERCENT` for the crop and soil after observation. Use `ON < OFF` so hysteresis works as intended.

## ThingSpeak fields
| Field | Value |
|---|---|
| Field 1 | Soil moisture (%) |
| Field 2 | Temperature (°C), if DHT enabled |
| Field 3 | Relative humidity (%), if DHT enabled |
| Field 4 | Pump state (1=ON, 0=OFF) |
| Field 5 | Tank available (1=yes, 0=no; 1 if no tank sensor fitted) |

ThingSpeak free accounts can have upload interval limits. The firmware defaults to a 20-second update interval; adjust it to your account/platform requirements.

## Troubleshooting
- **Moisture always 0% or 100%:** recalibrate raw dry/wet values and confirm the sensor output is within the ESP32 ADC input range.
- **Pump logic inverted:** verify relay active-low setting and test with the pump disconnected.
- **ESP32 resets when pump starts:** use an appropriately rated separate pump supply, common ground where required by the driver, and proper suppression per the pump/driver datasheets.
- **No ThingSpeak updates:** check Wi-Fi, channel field setup, API key, and update interval. Local control should still work.
- **DHT compile error:** install Adafruit's DHT sensor library or set `USE_DHT` to `0`.

## GitHub upload
Create a new empty GitHub repository, then from this folder run:
```bash
git init
git add .
git commit -m "Initial smart irrigation project"
git branch -M main
git remote add origin https://github.com/YOUR-USERNAME/YOUR-REPOSITORY.git
git push -u origin main
```
Replace the URL with your repository URL. Do not commit real Wi-Fi passwords or API keys.

## Limitations
One sensor represents only a small area. Moisture readings require calibration, and sensor drift is possible. Wi-Fi/cloud reporting depends on connectivity. Test and document actual results; do not claim water savings or crop-yield improvement without measured trials.
