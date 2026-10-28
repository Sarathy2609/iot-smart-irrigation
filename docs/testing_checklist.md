# Testing Checklist

Complete each test with the pump disconnected first, then repeat using a safe low-voltage setup.

| Test | Setup | Expected result | Observed result |
|---|---|---|---|
| 1 | Dry soil; tank has water | Pump requests ON below ON threshold | Fill after test |
| 2 | Moist soil at/above OFF threshold | Pump OFF | Fill after test |
| 3 | Tank switch indicates empty | Pump OFF | Fill after test |
| 4 | Wi-Fi disconnected | Local control continues | Fill after test |
| 5 | Invalid calibration configuration | Pump OFF and fault message | Fill after test |
| 6 | Pump on longer than max runtime | Pump stops | Fill after test |
| 7 | ThingSpeak configured and Wi-Fi available | Channel updates appear | Fill after test |
| 8 | Power cycle | Pump starts OFF | Fill after test |

## Results to record
- Dry-soil and wet-soil raw ADC values
- Calibrated moisture percentage range
- Pump response time
- Water volume per irrigation cycle
- Cloud update interval and connection reliability
- Any relay chatter, sensor drift, leaks, or resets
