#include <WiFi.h>
#include <HTTPClient.h>

char* WIFI_SSID = "SSID";
char* password = "PASSWORD";

char* serverName = "TO-DO";

unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting...\n");
  }
  Serial.print("Connected. IP Address: ");
  Serial.println(WiFi.localIP());

void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String Data_to_send = "weight=30&day=30"; //example format
      // Send HTTP POST request
      int ResponseCode = http.POST(Data_to_send);


      Serial.print("Code Returned: ");
      Serial.println(ResponseCode);

      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
