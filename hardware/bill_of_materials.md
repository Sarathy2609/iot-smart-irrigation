# Bill of Materials

| Item | Quantity | Purpose | Notes |
|---|---:|---|---|
| ESP32 development board | 1 | Sensor reading, control, Wi-Fi | Use a 3.3 V-compatible board |
| Capacitive soil moisture sensor | 1 | Analog moisture estimate | Prefer capacitive over exposed-resistive type |
| Relay module or MOSFET driver | 1 | Switches pump | Must be rated for pump voltage/current |
| Low-voltage DC pump | 1 | Water delivery | Match pump supply rating |
| Pump DC power supply | 1 | Powers pump | Separate from ESP32 supply as appropriate |
| Jumper wires/breadboard | As needed | Connections | Keep away from water |
| Tubing and water container | As needed | Irrigation path | Secure against leaks |
| DHT22 (optional) | 1 | Ambient temperature/humidity | Set `USE_DHT 1` and wire to DHT_PIN |
| Float switch (optional) | 1 | Empty-tank interlock | Firmware assumes LOW = water available |

Use component datasheets to verify voltage, current, polarity, and safe wiring. A relay module may need a suitable driver and supply; do not assume every module is ESP32-compatible.
