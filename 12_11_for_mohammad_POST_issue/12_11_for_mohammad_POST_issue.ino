#include <Arduino.h>
#include "HX711.h"
#include "soc/rtc.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"

#define PIN_INTERRUPT 21
#define MAX_DATA_PER_DAY 100
char* WIFI_SSID = "Mohammad";
char* password = "Hamoor221";
char* serverName = "http://helloflops.com/submit";

unsigned long timerDelay = 5000;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 35;
const int LOADCELL_SCK_PIN = 36;

HX711 scale;

double weightArray[MAX_DATA_PER_DAY];  // Array to store weight measurements
int weightIndex = 0;  // To track number of stored weights
unsigned long lastTime = 0;

const int interruptPin = PIN_INTERRUPT;

// Helper function to get zero-padded strings for date/time components
String twoDigitString(int value) {
  if (value < 10) {
    return "0" + String(value);
  } else {
    return String(value);
  }
}

void sendWeight(double weight, String dateStr) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverName);

    String weightStr = String(weight, DEC);
    // Data in the format "weight=...&date=YYYY-MM-DD HH:MM"
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
  scale.power_up();
  if (digitalRead(interruptPin) == HIGH) {
    // Get current time
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }

    int currentHour = timeinfo.tm_hour;
    int currentMinute = timeinfo.tm_min;

    // Check if the time is within 30 minutes of midnight (00:00)
    bool isNearMidnight = 1;
    //(currentHour == 23 && currentMinute >= 30) || (currentHour == 0 && currentMinute == 0);

    // Build the date string with zero-padding
    String yearStr = String(timeinfo.tm_year + 1900);
    String monthStr = twoDigitString(timeinfo.tm_mon + 1);
    String dayStr = twoDigitString(timeinfo.tm_mday);
    String hourStr = twoDigitString(timeinfo.tm_hour);
    String minuteStr = twoDigitString(timeinfo.tm_min);

    String dateStr = yearStr + "-" + monthStr + "-" + dayStr + " " + hourStr + ":" + minuteStr;

    scale.power_up();

    double weight = scale.get_units();

    if (isNearMidnight) {
      if (weightIndex == 0) {
        Serial.println("Weight index is zero, sending data");
        // If the array is empty, send the current weight
        sendWeight(weight, dateStr);
      } else {
        // If there's data in the array, calculate and send the average weight
        calculateAndSendAverage(dateStr);
      }
    } else {
      // Store weight in the array if not within 30 minutes of midnight
      Serial.println("In weight array loop");
      if (weightIndex < MAX_DATA_PER_DAY) {
        weightArray[weightIndex] = weight;
        weightIndex++;
      } else {
        // If the array is full, calculate and send average
        calculateAndSendAverage(dateStr);
      }
    }

  }
  // Go back to light sleep after the interrupt is processed (if needed)
  // esp_light_sleep_start();
}
