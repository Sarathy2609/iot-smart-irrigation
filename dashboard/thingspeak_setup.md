# ThingSpeak Dashboard Setup

1. Create/sign in to a ThingSpeak account at https://thingspeak.com/.
2. Create a new channel named `Smart Irrigation`.
3. Enable these fields:
   - Field 1: Soil Moisture (%)
   - Field 2: Temperature (°C)
   - Field 3: Relative Humidity (%)
   - Field 4: Pump State (1=ON, 0=OFF)
   - Field 5: Tank Available (1=yes, 0=no)
4. Save the channel and copy its **Write API Key**.
5. In `firmware/smart_irrigation.ino`, replace `YOUR_THINGSPEAK_WRITE_API_KEY`.
6. Add your Wi-Fi SSID/password locally. Do not publish credentials in a public repository.
7. Set `USE_DHT 1` only if a DHT22 is connected and the library is installed; otherwise fields 2 and 3 are not sent.
8. Upload the sketch and open Serial Monitor at 115200 baud.
9. Confirm new channel entries appear. If not, check Wi-Fi, API key, field configuration, and platform update limits.

The firmware attempts uploads periodically but irrigation control runs locally. For public repositories, use a private local copy or a secrets-management approach for real credentials.
