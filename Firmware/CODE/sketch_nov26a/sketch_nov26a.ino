#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include <esp_sleep.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

const char* authURL = "http://192.168.1.1:5000/auth";
const char* dataURL = "http://192.168.1.1:5000/data";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 2 * 3600;
const int daylightOffset_sec = 0;

#define SIGNAL_PIN 13

RTC_DATA_ATTR String apiKey = "";
RTC_DATA_ATTR bool authenticated = false;

void setup() {
  Serial.begin(115200);
  pinMode(SIGNAL_PIN, INPUT_PULLDOWN);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, HIGH);

  if (!authenticated) {
    connectToWiFi();
    setupTime();
    authenticate();
  } else {
    sendPostRequest();
  }

  enterDeepSleep();
}

void loop() {}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void setupTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    return;
  }
}

String getCurrentTime() {
  struct tm timeInfo;
  if (getLocalTime(&timeInfo)) {
    char timeString[25];
    strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%S", &timeInfo);
    return String(timeString);
  }
  return "N/A";
}

String getUniqueID() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String macAddress = "";
  for (int i = 0; i < 6; i++) {
    macAddress += String(mac[i], HEX);
  }
  macAddress.toUpperCase();  // Make it uppercase for consistency
  return macAddress;
}

void authenticate() {
  HTTPClient http;
  http.begin(authURL);
  http.addHeader("Content-Type", "application/json");

  String uniqueID = getUniqueID();

  StaticJsonDocument<200> doc;
  doc["unique_id"] = uniqueID;

  String jsonData;
  serializeJson(doc, jsonData);

  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    String response = http.getString();
    StaticJsonDocument<200> responseDoc;
    DeserializationError error = deserializeJson(responseDoc, response);

    if (!error) {
      apiKey = responseDoc["api_key"].as<String>();
      authenticated = true;
    }
  }

  http.end();
}

void sendPostRequest() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  HTTPClient http;
  http.begin(dataURL);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + apiKey);

  StaticJsonDocument<200> doc;
  doc["timestamp"] = getCurrentTime();
  doc["pin_state"] = "HIGH";

  String jsonData;
  serializeJson(doc, jsonData);

  int httpResponseCode = http.POST(jsonData);

  http.end();
}

void enterDeepSleep() {
  delay(100);
  esp_deep_sleep_start();
}
