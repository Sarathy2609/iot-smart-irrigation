/*
  IoT-Based Smart Irrigation and Soil Management System
  Target: ESP32 DevKit
  Required: capacitive soil moisture sensor, relay/driver, low-voltage DC pump
  Optional: DHT22 and tank float switch

  SAFETY:
  - Never drive a pump directly from an ESP32 GPIO.
  - Use a properly rated driver and pump power supply.
  - Test with pump disconnected before wiring the load.
  - Confirm relay polarity and all voltage/current ratings.
*/

#include <WiFi.h>
#include <HTTPClient.h>

// ---------- USER CONFIGURATION ----------
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* THINGSPEAK_WRITE_API_KEY = "YOUR_THINGSPEAK_WRITE_API_KEY";

// Set to 1 only if the Adafruit DHT library and a DHT22 are installed.
#define USE_DHT 0
#if USE_DHT
  #include <DHT.h>
  #define DHT_PIN 4
  #define DHT_TYPE DHT22
  DHT dht(DHT_PIN, DHT_TYPE);
#endif

// ESP32 pins (change to match your board/wiring)
const int SOIL_PIN = 34;       // ADC input only pin on many ESP32 DevKits
const int RELAY_PIN = 26;      // Relay/driver input
const int TANK_PIN = 27;       // Optional float switch to GND when tank has water
const bool USE_TANK_SENSOR = true;

// Many relay modules are active-low. Verify your specific module.
const bool RELAY_ACTIVE_LOW = true;

// Calibrate these using your sensor. Example raw ADC values only.
const int MOISTURE_DRY_RAW = 3000;
const int MOISTURE_WET_RAW = 1400;

// Moisture percentage thresholds; adjust for crop/soil after testing.
const int MOISTURE_ON_PERCENT = 35;   // Start watering below this value
const int MOISTURE_OFF_PERCENT = 50;  // Stop watering at/above this value

const unsigned long SAMPLE_INTERVAL_MS = 2000;
const unsigned long UPLOAD_INTERVAL_MS = 20000;
const unsigned long MAX_PUMP_RUN_MS = 30000; // safety limit; tune for your setup
const unsigned long WIFI_RETRY_INTERVAL_MS = 10000;

// Set false if your float switch wiring differs.
// Current wiring assumption: INPUT_PULLUP and LOW means tank has water.
const bool TANK_SWITCH_LOW_MEANS_WATER = true;

// ---------- STATE ----------
bool pumpOn = false;
unsigned long pumpStartedAt = 0;
unsigned long lastSampleAt = 0;
unsigned long lastUploadAt = 0;
unsigned long lastWiFiAttemptAt = 0;

int moisturePercent = 0;
int soilRaw = 0;
bool tankAvailable = true;
bool sensorValid = false;

void setPump(bool on) {
  pumpOn = on;
  const int onLevel = RELAY_ACTIVE_LOW ? LOW : HIGH;
  const int offLevel = RELAY_ACTIVE_LOW ? HIGH : LOW;
  digitalWrite(RELAY_PIN, on ? onLevel : offLevel);
  if (on) {
    pumpStartedAt = millis();
  }
}

int rawToPercent(int raw) {
  // Works whether the wet raw value is lower or higher than the dry raw value.
  float pct = 100.0f * (float)(MOISTURE_DRY_RAW - raw) /
              (float)(MOISTURE_DRY_RAW - MOISTURE_WET_RAW);
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  return (int)(pct + 0.5f);
}

bool readTankAvailable() {
  if (!USE_TANK_SENSOR) return true;
  int state = digitalRead(TANK_PIN);
  return TANK_SWITCH_LOW_MEANS_WATER ? (state == LOW) : (state == HIGH);
}

void connectWiFiIfNeeded() {
  if (WiFi.status() == WL_CONNECTED) return;
  if (millis() - lastWiFiAttemptAt < WIFI_RETRY_INTERVAL_MS) return;
  lastWiFiAttemptAt = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Wi-Fi reconnect attempt; local irrigation control remains active.");
}

void uploadToThingSpeak() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (String(THINGSPEAK_WRITE_API_KEY).length() < 5 ||
      String(THINGSPEAK_WRITE_API_KEY) == "YOUR_THINGSPEAK_WRITE_API_KEY") {
    Serial.println("ThingSpeak API key not configured; skipping upload.");
    return;
  }

  HTTPClient http;
  String url = "http://api.thingspeak.com/update?api_key=";
  url += THINGSPEAK_WRITE_API_KEY;
  url += "&field1=" + String(moisturePercent);
#if USE_DHT
  float tempC = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (!isnan(tempC)) url += "&field2=" + String(tempC, 1);
  if (!isnan(humidity)) url += "&field3=" + String(humidity, 1);
#endif
  url += "&field4=" + String(pumpOn ? 1 : 0);
  url += "&field5=" + String(tankAvailable ? 1 : 0);

  http.setTimeout(4000);
  http.begin(url);
  int status = http.GET();
  Serial.printf("ThingSpeak HTTP status: %d\n", status);
  http.end();
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  setPump(false); // pump OFF at startup
  if (USE_TANK_SENSOR) pinMode(TANK_PIN, INPUT_PULLUP);
#if USE_DHT
  dht.begin();
#endif
  analogReadResolution(12);
  analogSetPinAttenuation(SOIL_PIN, ADC_11db);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lastWiFiAttemptAt = millis();

  Serial.println("\nSmart irrigation controller starting.");
  Serial.println("Calibrate dry/wet ADC values before using pump control.");
}

void loop() {
  unsigned long now = millis();
  connectWiFiIfNeeded();

  if (now - lastSampleAt >= SAMPLE_INTERVAL_MS) {
    lastSampleAt = now;
    soilRaw = analogRead(SOIL_PIN);
    // ESP32 ADC readings are normally 0..4095. Treat impossible readings as a fault.
    sensorValid = (soilRaw >= 0 && soilRaw <= 4095 &&
                   MOISTURE_DRY_RAW != MOISTURE_WET_RAW);
    tankAvailable = readTankAvailable();

    if (sensorValid) {
      moisturePercent = rawToPercent(soilRaw);
    } else {
      setPump(false);
      Serial.println("FAULT: invalid soil sensor/configuration; pump forced OFF.");
    }

    // Safety timeout takes priority over moisture logic.
    if (pumpOn && (now - pumpStartedAt >= MAX_PUMP_RUN_MS)) {
      setPump(false);
      Serial.println("SAFETY: maximum pump run time reached; pump OFF.");
    } else if (!sensorValid || !tankAvailable) {
      if (pumpOn) setPump(false);
      if (!tankAvailable) Serial.println("Tank empty/no-water signal: pump OFF.");
    } else if (!pumpOn && moisturePercent < MOISTURE_ON_PERCENT) {
      setPump(true);
      Serial.println("Soil dry: pump ON.");
    } else if (pumpOn && moisturePercent >= MOISTURE_OFF_PERCENT) {
      setPump(false);
      Serial.println("Moisture target reached: pump OFF.");
    }

    Serial.printf("Raw=%d Moisture=%d%% Tank=%s Pump=%s WiFi=%s\n",
                  soilRaw, moisturePercent,
                  tankAvailable ? "available" : "empty",
                  pumpOn ? "ON" : "OFF",
                  WiFi.status() == WL_CONNECTED ? "connected" : "offline");
  }

  if (now - lastUploadAt >= UPLOAD_INTERVAL_MS) {
    lastUploadAt = now;
    uploadToThingSpeak();
  }

  delay(10);
}
