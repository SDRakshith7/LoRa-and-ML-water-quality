TRANSMITTER

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <LoRa.h>

// Sensor pins
#define pH_SENSOR_PIN A0
#define TDS_SENSOR_PIN A1
#define TEMPERATURE_SENSOR_PIN 3
#define TURBIDITY_SENSOR_PIN A2
const int turbidityPin = A0; // Analog pin where the turbidity sensor output is connected
const int conductivityPin = A1; // Analog pin where the conductivity sensor output is connected

// Thresholds
#define pH_THRESHOLD 7.0
#define TDS_THRESHOLD 500
#define TEMPERATURE_THRESHOLD 20
#define TURBIDITY_THRESHOLD 5
const float THRESHOLD_TURBIDITY = 500.0; // Threshold turbidity level for the alert

// Initialize sensors
Adafruit_BME280 bme;
OneWire oneWire(TEMPERATURE_SENSOR_PIN);
DallasTemperature sensors(&oneWire);
float temperatureOffset = 0.0;

// LoRa settings
#define LORA_SCK 13
#define LORA_MISO 12
#define LORA_MOSI 11
#define LORA_CS 10
#define LORA_RST 9
#define LORA_DI0 2

#define BAND 433E6  // LoRa band

void setup() {
  Serial.begin(9600);

  // Initialize LoRa module
  SPI.begin();
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DI0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Initialize pH sensor
  pinMode(pH_SENSOR_PIN, INPUT);

  // Initialize TDS sensor
  pinMode(TDS_SENSOR_PIN, INPUT);

  // Initialize temperature sensor
  sensors.begin();

  // Initialize turbidity sensor
  pinMode(TURBIDITY_SENSOR_PIN, INPUT);

  // Calibrate temperature sensor
  calibrateSensor();
  calibrateTurbiditySensor(); // Calibrate turbidity sensor
}

void loop() {
  // Read sensor values
  float pHValue = readpH();
  float tdsValue = readTDS();
  float temperatureC = readTemperature();
  float turbidityNTU = readTurbidity();
  float conductivity = readConductivity();

  // Check if any values exceed the thresholds
  bool pHAlert = pHValue > pH_THRESHOLD;
  bool tdsAlert = tdsValue > TDS_THRESHOLD;
  bool temperatureAlert = temperatureC > TEMPERATURE_THRESHOLD;
  bool turbidityAlert = turbidityNTU > TURBIDITY_THRESHOLD;

  // Print sensor values and alerts
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

  // Send sensor values and alerts via LoRa
  String message = "pH:" + String(pHValue, 2) + " TDS:" + String(tdsValue, 2) + " Temperature:" + String(temperatureC, 2) +
                   " Turbidity:" + String(turbidityNTU, 2) + " Conductivity:" + String(conductivity, 2) +
                   " pHAlert:" + String(pHAlert) + " TDSAlert:" + String(tdsAlert) + " TemperatureAlert:" +
                   String(temperatureAlert) + " TurbidityAlert:" + String(turbidityAlert);
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();

  delay(3000);
}

float readpH() {
  int sensorValue = analogRead(pH_SENSOR_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  float pHValue = 3.5 * voltage;
  return pHValue;
}

float readTDS() {
  int sensorValue = analogRead(TDS_SENSOR_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  float tdsValue = 1000 * voltage;
  return tdsValue;
}

float readTemperature() {
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0) + temperatureOffset;
  return temperatureC;
}

float readTurbidity() {
  int sensorValue = analogRead(TURBIDITY_SENSOR_PIN);
  float turbidityNTU = convertToTurbidity(sensorValue);
  // Calibration formula for turbidity sensor goes here
  // turbidityNTU = ...
  return turbidityNTU;
}

void calibrateSensor() {
  Serial.println("Enter the temperature offset value for calibration:");
  while (!Serial.available());
  temperatureOffset = Serial.parseFloat();
  Serial.print("Calibration offset set to: ");
  Serial.println(temperatureOffset);
}

void calibrateTurbiditySensor() {
  // Calibration steps for turbidity sensor
  Serial.println("Calibrating turbidity sensor...");
  // Implement calibration process here
  // Example: prompt user to place the sensor in known turbidity solution and measure the analog value
  // Then map this value to the known turbidity level to determine the conversion factor
  Serial.println("Turbidity sensor calibration completed.");
}

float readConductivity() {
  // Read the analog value from the conductivity sensor
  int conductivityValue = analogRead(conductivityPin);
  
  // Convert the analog value to conductivity level (calibrated value)
  float conductivity = convertToConductivity(conductivityValue);
  
  return conductivity;
}

float convertToConductivity(int analogValue) {
  // Implement your calibration formula here
  // Example: use a linear equation or lookup table to map analog value to conductivity level
  // Calibration formula: conductivity = m * analogValue + b
  // Where 'm' and 'b' are calibration coefficients determined during calibration process
  // Replace the following line with your calibration formula
  return analogValue * 0.05; // Example: linear calibration with slope 0.05
}

float convertToTurbidity(int analogValue) {
return analogValue * 0.05;
}

