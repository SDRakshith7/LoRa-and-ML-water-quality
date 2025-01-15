RECEIVER


#define BLYNK_TEMPLATE_ID "TMPL3bZRv37eU"
#define BLYNK_TEMPLATE_NAME "Water"
#define BLYNK_AUTH_TOKEN "Your Blynk Auth Token"

#include <SPI.h>
#include <LoRa.h>
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>

#define BAND 433E6  // LoRa band

// LoRa settings
#define LORA_SCK 13
#define LORA_MISO 12
#define LORA_MOSI 11
#define LORA_CS 10
#define LORA_RST 9
#define LORA_DI0 2

// WiFi credentials
char ssid[] = "Your SSID";
char pass[] = "Your Password";

// Blynk authentication token
char auth[] = "Your Blynk Auth Token";

void setup() {
  Serial.begin(9600);

  // Initialize LoRa module
  SPI.begin();
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DI0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Initialize WiFi
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  // Check if a packet has been received
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.println("Received packet:");

    // Read the received message
    String message = "";
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }

    // Print the received message
    Serial.println(message);

    // Parse the message and extract sensor values and alerts
    float pHValue = 0.0, tdsValue = 0.0, temperatureC = 0.0, turbidityNTU = 0.0, conductivity = 0.0;
    bool pHAlert = false, tdsAlert = false, temperatureAlert = false, turbidityAlert = false;

    // Example parsing, modify as needed for your message format
    int index = message.indexOf(' ');
    while (index != -1) {
      String parameter = message.substring(0, index);
      message = message.substring(index + 1);
      index = message.indexOf(' ');

      int colonIndex = parameter.indexOf(':');
      String paramName = parameter.substring(0, colonIndex);
      String paramValue = parameter.substring(colonIndex + 1);

      if (paramName == "pH") {
        pHValue = paramValue.toFloat();
      } else if (paramName == "TDS") {
        tdsValue = paramValue.toFloat();
      } else if (paramName == "Temperature") {
        temperatureC = paramValue.toFloat();
      } else if (paramName == "Turbidity") {
        turbidityNTU = paramValue.toFloat();
      } else if (paramName == "Conductivity") {
        conductivity = paramValue.toFloat();
      } else if (paramName == "pHAlert") {
        pHAlert = paramValue.toInt();
      } else if (paramName == "TDSAlert") {
        tdsAlert = paramValue.toInt();
      } else if (paramName == "TemperatureAlert") {
        temperatureAlert = paramValue.toInt();
      } else if (paramName == "TurbidityAlert") {
        turbidityAlert = paramValue.toInt();
      }
    }

    // Print received values and alerts
    Serial.print("pH: ");
    Serial.print(pHValue);
    Serial.print(" ");
    Serial.print(pHAlert ? "Alert" : "OK");
    Serial.println();

    Serial.print("TDS: ");
    Serial.print(tdsValue);
    Serial.print(" ppm ");
    Serial.print(tdsAlert ? "Alert" : "OK");
    Serial.println();

    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.print(" °C ");
    Serial.print(temperatureAlert ? "Alert" : "OK");
    Serial.println();

    Serial.print("Turbidity: ");
    Serial.print(turbidityNTU);
    Serial.print(" NTU ");
    Serial.print(turbidityAlert ? "Alert" : "OK");
    Serial.println();

    Serial.print("Conductivity: ");
    Serial.print(conductivity);
    Serial.println(" µS/cm");

    // Send sensor values and alerts to Blynk
    Blynk.virtualWrite(V1, pHValue);
    Blynk.virtualWrite(V2, tdsValue);
    Blynk.virtualWrite(V3, temperatureC);
    Blynk.virtualWrite(V4, turbidityNTU);
    Blynk.virtualWrite(V5, conductivity);
    Blynk.virtualWrite(V6, pHAlert);
    Blynk.virtualWrite(V7, tdsAlert);
    Blynk.virtualWrite(V8, temperatureAlert);
    Blynk.virtualWrite(V9, turbidityAlert);
  }

  Blynk.run();
}





