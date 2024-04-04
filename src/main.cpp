#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <rgb_lcd.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <SRAM.h>
#include <SPI.h>

#define DHTTYPE DHT11
#define DHTPIN 2
// Define standard values to measure against the temperature
#define minTemperature 23.00
#define maxTemperature 26.00
#define minHumidity 45.00
#define maxHumidity 55.00

DHT dhtSensor(DHTPIN, DHTTYPE);
rgb_lcd lcdScreen;

byte macAddress[] = {
  0xFF, 0xDE, 0xFE, 0xDD, 0xED, 0xEF
};
IPAddress ipAddress(192, 168, 1, 2);

void setup() {
  Serial.begin(9600);
  lcdScreen.begin(16, 2);
  dhtSensor.begin();
  Ethernet.begin(macAddress, ipAddress);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("No ethernet shield found. Verify that ethernet shield is connected, and try again.");
    return;
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("No ethernet cable is connected. Verify the connection and try again.");
    return;
  }
}

void loop() {
  float humidity = dhtSensor.readHumidity();
  float temperature = dhtSensor.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed reading from the DHT sensor. Please check pin connection!");
    return;
  }
  float minTempResult = minTemperature - temperature;
  float minHumResult = minHumidity - humidity;
  float maxTempResult = temperature - maxTemperature;
  float maxHumResult = humidity - maxHumidity;
  // Lots of prints used for debugging...
  Serial.print("Current Temperature - Minimum Temperature = ");
  Serial.println(minTempResult);
  Serial.print("Current Humidity - Minimum Humidity = ");
  Serial.println(minHumResult);
  Serial.print("Maximum Temperature - Current Temperature = ");
  Serial.println(maxTempResult);
  Serial.print("Maximum Humidity - Current Humidity = ");
  Serial.println(maxHumResult);

  // Set colors on the LCD depending on the severity of the temperature
  if ((minTempResult >= 10 || maxTempResult >= 10) || (minHumResult >= 20 || maxHumResult >= 20)) {
    Serial.println("Critical threshhold reached...");
    lcdScreen.setRGB(255, 0, 0);
  } else if ((minTempResult >= 2 || maxTempResult >= 2) || (minHumResult >= 10 || maxHumResult >= 10)) {
    Serial.println("Warning threshhold reached...");
    lcdScreen.setRGB(155, 155, 0);
  } else {
    lcdScreen.setRGB(0, 255, 0);
  }

  // Print to LCD
  lcdScreen.setCursor(0, 0);
  lcdScreen.print("Hum: ");
  lcdScreen.print(humidity);
  lcdScreen.print("%");
  lcdScreen.setCursor(0, 1);
  lcdScreen.print("Temp: ");
  lcdScreen.print(temperature);

  // Print to Serial output
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("% \nTemperature: ");
  Serial.println(temperature);

  delay(5000);
}

// put function definitions here: