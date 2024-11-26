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

/**
 * @brief Sets up the ESP32 microcontroller
 * 
 * This function initializes the ESP32 board by setting up the serial port, 
 * setting the pin mode for the signal pin, and enabling the external wake-up 
 * feature using the esp_sleep_enable_ext0_wakeup function. It also calls the 
 * setupTime function to set up the time and authenticate the device using the 
 * authenticate function. If authentication fails, it prints an error message to the serial port.
 * Finally, it sends a POST request to the server using the sendPostRequest function and enters deep sleep.
 */
void setup() {
  Serial.begin(115200);
  pinMode(SIGNAL_PIN, INPUT_PULLDOWN);

  // Check if esp_sleep_enable_ext0_wakeup is available
  if (&esp_sleep_enable_ext0_wakeup != nullptr) {
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, HIGH);
  } else {
    Serial.println("Error: esp_sleep_enable_ext0_wakeup is null");
    return;
  }

  try {
    if (!authenticated) {
      connectToWiFi();
      setupTime();
      authenticate();
    } else {
      sendPostRequest();
    }
  } catch (const std::exception& e) {
    Serial.print("Exception: ");
    Serial.println(e.what());
    return;
  } catch (...) {
    Serial.println("Unknown error occurred");
    return;
  }

  enterDeepSleep();
}

void loop() {}

/**
 * @brief Connects to the WiFi network
 * 
 * This function attempts to connect to a WiFi network using the provided SSID
 * and password. It prints an error message to the serial port if the SSID or
 * password is null. It also prints an error message if the connection attempt
 * fails after a certain number of attempts.
 */
void connectToWiFi() {
  // Check if the SSID or password is null
  if (ssid == nullptr || password == nullptr) {
    Serial.println("Error: SSID or password is null");
    return;
  }

  // Attempt to connect to the WiFi network
  WiFi.begin(ssid, password);

  int attempts = 0;
  const int maxAttempts = 20;

  // Wait for the connection to be established or max attempts reached
  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    delay(500);
    attempts++;
  }

  // Check if the connection was established
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Error: Failed to connect to WiFi");
    return;
  }

  Serial.println("Connected to WiFi");
}

/**
 * @brief Sets up the time using NTP server
 * 
 * This function configures the ESP32's internal clock to synchronize with an NTP server. 
 * It checks if the ntpServer is not null before proceeding. If the setup fails, it prints 
 * appropriate error messages to the serial port.
 */
void setupTime() {
  // Check if the NTP server address is provided
  if (ntpServer == nullptr) {
    Serial.println("Error: ntpServer is null");
    return;
  }

  // Configure the time using the specified NTP server and offset values
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeInfo;
  // Attempt to get the local time
  if (!getLocalTime(&timeInfo)) {
    Serial.println("Error: Failed to get local time");
    return;
  }
}

/**
 * @brief Returns the current time as a string in ISO 8601 format
 * 
 * This function allocates memory for a tm struct and uses the getLocalTime
 * function to fill it with the current time. It then uses strftime to convert
 * the tm struct to a string in ISO 8601 format. Finally, it deallocates the
 * memory and returns the string.
 * 
 * @return String representing the current time in ISO 8601 format
 */
String getCurrentTime() {
  struct tm* timeInfo = new struct tm; // Allocate memory to prevent null pointer references
  if (getLocalTime(timeInfo)) {
    char timeString[25];
    strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%S", timeInfo);
    delete timeInfo; // Deallocate memory to prevent memory leaks
    return String(timeString);
  }
  delete timeInfo; // Deallocate memory to prevent memory leaks
  return "N/A";
}

/**
 * @brief Returns the unique ID as a string of the device's MAC address
 * 
 * This function retrieves the MAC address of the ESP32 board and converts it
 * to a string in hexadecimal format. The string is then returned as the unique
 * ID of the device.
 * 
 * @return String representing the unique ID of the device
 */
String getUniqueID() {
  uint8_t mac[6];

  // Check if WiFi is null
  if (WiFi == nullptr) {
    Serial.println("Error: WiFi is null");
    return "";
  }

  // Check if WiFi.macAddress is null
  if (WiFi.macAddress == nullptr) {
    Serial.println("Error: WiFi.macAddress is null");
    return "";
  }

  // Attempt to get the MAC address
  if (!WiFi.macAddress(mac)) {
    Serial.println("Error: Failed to get MAC address");
    return "";
  }

  String macAddress = "";
  for (int i = 0; i < 6; i++) {
    macAddress += String(mac[i], HEX); // Convert each byte to hexadecimal
  }

  // Make the MAC address string uppercase for consistency
  macAddress.toUpperCase();
  return macAddress;
}

/**
 * @brief Authenticates the device and retrieves an API key
 * 
 * This function sends a POST request to the authentication URL with the
 * unique ID of the device. It retrieves the API key from the response and
 * stores it in the global apiKey variable. It also sets the authenticated
 * flag to true.
 * 
 * @return void
 */
void authenticate() {
  HTTPClient http;
  http.begin(authURL);
  http.addHeader("Content-Type", "application/json");

  String uniqueID = getUniqueID();

  // Check if uniqueID is empty
  if (uniqueID == "") {
    Serial.println("Error: Unique ID is empty");
    return;
  }

  StaticJsonDocument<200> doc;
  doc["unique_id"] = uniqueID;

  String jsonData;
  serializeJson(doc, jsonData);

  int httpResponseCode = http.POST(jsonData);

  // Check for errors
  if (httpResponseCode <= 0) {
    Serial.print("Error: ");
    Serial.println(http.errorString(httpResponseCode));
    return;
  }

  String response = http.getString();

  // Check if response is empty
  if (response == "") {
    Serial.println("Error: Response is empty");
    return;
  }

  StaticJsonDocument<200> responseDoc;
  DeserializationError error = deserializeJson(responseDoc, response);

  // Check for deserialization errors
  if (error) {
    Serial.print("Deserialization error: ");
    Serial.println(error.c_str());
    return;
  }

  // Check if api_key is present in the response
  if (!responseDoc.containsKey("api_key")) {
    Serial.println("Error: Response does not contain an API key");
    return;
  }

  apiKey = responseDoc["api_key"].as<String>();
  authenticated = true;
}

/**
 * @brief Sends a POST request to the server with the current timestamp and the state of the signal pin
 * 
 * This function sends a POST request to the server with the current timestamp and the state of the signal pin. 
 * It checks if the apiKey is not null before adding the Authorization header to the HTTP request. 
 * It also checks if SIGNAL_PIN is not null before reading its state and adding it to the JSON document. 
 * Finally, it catches any exceptions that may occur during the execution of the function.
 */
void sendPostRequest() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  HTTPClient http;
  http.begin(dataURL);
  http.addHeader("Content-Type", "application/json");

  // Check if apiKey is not null
  if (apiKey == nullptr) {
    Serial.println("Error: apiKey is null");
    return;
  }
  http.addHeader("Authorization", "Bearer " + apiKey);

  StaticJsonDocument<200> doc;
  doc["timestamp"] = getCurrentTime();

  // Check if SIGNAL_PIN is not null
  if (SIGNAL_PIN == nullptr) {
    Serial.println("Error: SIGNAL_PIN is null");
    return;
  }
  doc["pin_state"] = digitalRead(SIGNAL_PIN) ? "HIGH" : "LOW";

  String jsonData;
  serializeJson(doc, jsonData);

  try {
    int httpResponseCode = http.POST(jsonData);
    if (httpResponseCode <= 0) {
      Serial.print("Error: ");
      Serial.println(http.errorString(httpResponseCode));
    }
  } catch (const std::exception& e) {
    Serial.print("Exception: ");
    Serial.println(e.what());
  } catch (...) {
    Serial.println("Unknown error occurred");
  }

  http.end();
}

/**
 * @brief Puts the ESP32 into deep sleep
 * 
 * This function attempts to put the ESP32 into deep sleep mode using the
 * esp_deep_sleep_start function. If the function is null, it prints an error
 * message to the serial port. It also catches any exceptions that may occur
 * during the execution of the function.
 */
void enterDeepSleep() {
  try {
    // Check if esp_deep_sleep_start is not null
    if (esp_deep_sleep_start != nullptr) {
      delay(100); // Wait for a short period of time before entering deep sleep
      esp_deep_sleep_start(); // Enter deep sleep
    } else {
      Serial.println("Error: esp_deep_sleep_start is null");
    }
  } catch (const std::exception& e) {
    Serial.print("Exception: ");
    Serial.println(e.what()); // Print the exception message
  } catch (...) {
    Serial.println("Unknown error occurred");
  }
}

