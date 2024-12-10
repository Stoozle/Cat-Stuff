#include <Arduino.h>
#include "HX711.h"
#include "soc/rtc.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"

#define PIN_INTERRUPT 21
#define MAX_DATA_PER_DAY 100
char* WIFI_SSID = "SSID";
char* password = "PASSWORD";
char* serverName = "TO-DO";

unsigned long timerDelay = 5000;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;

HX711 scale;

double weightArray[MAX_DATA_PER_DAY];  // Array to store weight measurements
int weightIndex = 0;  // To track number of stored weights
unsigned long lastTime = 0;

const int interruptPin = PIN_INTERRUPT;

void IRAM_ATTR interruptHandler() {
  // Check if the interrupt pin is HIGH
  if (digitalRead(interruptPin) == HIGH) {
    // Perform the regular functionality only if the pin is HIGH

    // Get current time
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;


    int currentHour = timeinfo.tm_hour;
    int currentMinute = timeinfo.tm_min;

    // Check if the time is within 30 minutes of midnight (00:00)
    bool isNearMidnight = (currentHour == 23 && currentMinute >= 30) || (currentHour == 0 && currentMinute == 0);
    String dateStr = String(timeinfo.tm_year + 1900) + "-" +
                     String(timeinfo.tm_mon + 1) + "-" +
                     String(timeinfo.tm_mday) + " " +
                     String(timeinfo.tm_hour) + ":" +
                     String(timeinfo.tm_min);

    scale.power_up();
    if (scale.is_ready()) {
      double weight = scale.get_units();

      if (isNearMidnight) {
        if (weightIndex == 0) {
          // If the array is empty, send the current weight
          sendWeight(weight, dateStr);
        } else {
          // If there's data in the array, calculate and send the average weight
          calculateAndSendAverage(dateStr);
        }
      } else {
        // Store weight in the array if not within 30 minutes of midnight
        if (weightIndex < MAX_DATA_PER_DAY) {
          weightArray[weightIndex] = weight;
          weightIndex++;
        } else {
          // If the array is full, calculate and send average
          calculateAndSendAverage(dateStr);
        }
      }
    } else {
      Serial.println("HX711 not ready.");
    }
  }
  }
  // Go back to light sleep after the interrupt is processed
  esp_light_sleep_start();
}

void sendWeight(double weight, String dateStr) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverName);

    String weightStr = String(weight, DEC);
    String dataToSend = "weight=" + weightStr + "&date=" + dateStr;

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int responseCode = http.POST(dataToSend);

    Serial.print("Code Returned: ");
    Serial.println(responseCode);

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void calculateAndSendAverage(String dateStr) {
  double totalWeight = 0;
  for (int i = 0; i < weightIndex; i++) {
    totalWeight += weightArray[i];
  }
  double averageWeight = totalWeight / weightIndex;

  // Reset the array for the next set of readings
  weightIndex = 0;

  sendWeight(averageWeight, dateStr);
}

void setup() {
  Serial.begin(115200);

  rtc_cpu_freq_config_t config;
  rtc_clk_cpu_freq_get_config(&config);
  rtc_clk_cpu_freq_set_config_fast(&config);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  pinMode(interruptPin, INPUT_PULLDOWN);
  // Attach interrupt handler to the pin (triggered on HIGH)
  attachInterrupt(digitalPinToInterrupt(interruptPin), interruptHandler, RISING);

  // Initialize WiFi
  WiFi.begin(WIFI_SSID, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting...\n");
  }
  Serial.print("Connected. IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize time
  configTime(0, 0, "pool.ntp.org"); // Synchronize time using NTP
}

void loop() {
  // Put the device into light sleep if no interrupt has triggered
  esp_light_sleep_start();
}
