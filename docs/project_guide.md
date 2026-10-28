# Project Guide: IoT-Based Smart Irrigation and Soil Management

## Abstract
The project monitors soil moisture with a sensor connected to an ESP32. The controller compares the reading with calibrated thresholds and switches a low-voltage pump through a suitable driver. Optional environmental and tank-level sensors extend monitoring and safety. Wi-Fi enables remote reporting to ThingSpeak, while local threshold control continues when the network is unavailable.

## Problem statement
Manual irrigation and fixed-time watering do not always reflect the moisture available around plant roots. This can lead to overwatering, underwatering, avoidable labour, and limited visibility into field conditions.

## Objectives
- Measure moisture in the plant root zone.
- Optionally read ambient temperature and humidity.
- Automate low-voltage pump control using calibrated thresholds.
- Publish readings remotely when network access is available.
- Add safety logic for tank availability and maximum pump runtime.

## Architecture
Soil moisture sensor (+ optional DHT22 and tank switch) → ESP32 → relay/driver → DC pump.
The ESP32 also sends readings to ThingSpeak over Wi-Fi when connected.

## Working principle
1. Initialise sensor, relay/driver, and network.
2. Sample soil sensor and map raw readings to a calibrated percentage.
3. Turn pump on below the ON threshold.
4. Turn pump off at/above the OFF threshold, or when tank/sensor/time safety conditions require it.
5. Upload telemetry periodically when Wi-Fi is available.
6. Continue local control if the cloud connection is unavailable.

## Expected results
The prototype is expected to demonstrate sensor readings, threshold-based pump switching, and optional remote telemetry. Record actual measurements during testing; do not substitute assumed values for observed results.

## Limitations
A single sensor may not represent an entire field. Calibration and sensor drift affect accuracy. Wi-Fi/cloud availability is not guaranteed, and the prototype does not by itself prove water savings or increased crop yield.

## Future enhancements
Multiple moisture sensors and irrigation zones, rain/forecast data, solar power, stronger authentication, and predictive recommendations based on collected field data.
