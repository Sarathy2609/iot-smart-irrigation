# Wiring Guide (ESP32 example)

**Disconnect all power before wiring. Verify every module's pin labels and datasheets.**

| Component | ESP32 / supply connection |
|---|---|
| Soil sensor VCC | Sensor-rated supply (check module; use 3.3 V if supported) |
| Soil sensor GND | ESP32 GND |
| Soil sensor analog output | GPIO34 (ADC input) |
| Relay/driver control input | GPIO26 |
| Relay/driver ground | ESP32 GND when required by the driver design |
| Pump | Through correctly rated relay contacts or MOSFET driver and its rated DC supply; never to GPIO |
| Optional float switch | One side GPIO27, other side GND; firmware uses `INPUT_PULLUP` and LOW = water available |
| Optional DHT22 DATA | GPIO4; use required pull-up resistor/module and set `USE_DHT 1` |

## Critical checks
1. Confirm the soil sensor's output never exceeds the ESP32 ADC input voltage range.
2. Confirm whether the relay input is active-low or active-high; set `RELAY_ACTIVE_LOW` accordingly.
3. Verify the pump's startup/stall current against the driver and supply ratings.
4. Use a separate pump supply if required. Common ground is needed for many low-side MOSFET drivers, but follow the specific driver design.
5. Add appropriate flyback/surge suppression as specified for the pump and driver.
6. Keep electronics dry; use an enclosure and strain relief.
7. Start with the pump disconnected and verify output behavior using Serial Monitor or a safe test load.
