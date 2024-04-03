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

DHT dhtSensor(DHTPIN, DHTTYPE);
rgb_lcd lcdScreen;

byte macAddress[] = {
  0xFF, 0xDE, 0xFE, 0xDD, 0xED, 0xEF
};
IPAddress ipAddress(192, 168, 1, 2);

void setup() {
  Serial.begin(9600);
  lcdScreen.begin(16, 2);
  lcdScreen.setRGB(0, 255, 0);
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
  // put your main code here, to run repeatedly:
  float humidity = dhtSensor.readHumidity();
  float temperature = dhtSensor.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed reading from the DHT sensor. Please check pin connection!");
    return;
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

  delay(2000);
}

// put function definitions here: