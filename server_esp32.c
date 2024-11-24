#include <Arduino.h>
#include "HX711.h"
#include "soc/rtc.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"

char* WIFI_SSID = "SSID";
char* password = "PASSWORD";

char* serverName = "TO-DO";

unsigned long timerDelay = 5000;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;

HX711 scale;

void setup() {
  Serial.begin(115200);
  rtc_cpu_freq_config_t config;
  rtc_clk_cpu_freq_get_config(&config);
  //rtc_clk_cpu_freq_to_config(RTC_CPU_FREQ_80M, &config);
  rtc_clk_cpu_freq_set_config_fast(&config);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  struct tm local_time;
  
  scale.set_scale(103.84); //value determined from calibration procedure
  scale.tare();               // reset the scale to 0
  
   WiFi.begin(WIFI_SSID, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting...\n");
  }
  Serial.print("Connected. IP Address: ");
  Serial.println(WiFi.localIP());
}
void loop() {
	if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      http.begin(client, serverName);
	  scale.power_up();
	  
		if (scale.is_ready()) {
			double weight_as_number = scale.get_units();
			string weight = String(weight_as_number, DEC);
			
			// Specify content-type header
			http.addHeader("Content-Type", "application/x-www-form-urlencoded");
			// Data to send with HTTP POST
			String Data_to_send = String("weight=" + weight + "&day=30"; //example format
			// Send HTTP POST request
			int ResponseCode = http.POST(Data_to_send);
		 
		
			Serial.print("Code Returned: ");
			Serial.println(ResponseCode);

			// Free resources
			http.end();
			scale.power_down();
		}
		else 
		{
			Serial.println("HX711 not found.");
		}
	}
	else
	{
		Serial.println("WiFi Disconnected");
    }
	lastTime = millis();
  }
}
  
  
