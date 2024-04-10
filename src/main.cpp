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
IPAddress server(192, 168, 1, 139);
EthernetClient ethClient;
PubSubClient client(ethClient);

/// @brief Convert a float to a string
/// @param x The variable to convert 
/// @param digits The amount of digits you want
/// @return The decimal in a String object
String decimal_to_string_float(unsigned int x, int digits) {
    String r = "";
    while((digits--)>0) {
        r = (char)(x%10+48)+r;
        x /= 10;
    }
    return r;
}

void reconnectClient() {
  // Loop until we're connected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("arduinoClient")) {
      Serial.println("connected");
    } else {
      Serial.print("failed: rc=");
      Serial.println(client.state());
    }
    delay(5000);
  }
}

void setup() {
  Serial.begin(9600);
  lcdScreen.begin(16, 2);
  dhtSensor.begin();

  client.setServer(server, 1883);
  Ethernet.begin(macAddress, ipAddress);

  // Wait a bit to let the hardware process connections
  delay(1500);

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
  // Establish MQTT connection before doing anything else
  if (!client.connected()) {
    reconnectClient();
  }
  client.loop();

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

  // Publish temperature and humidity readings to MQTT broker
  // TODO: Investigate a different way to convert temperature and humidity readings to a message WITHOUT using String class.
  // Dynamic allocation is a big no no for embedded systems with limited memory: https://forum.arduino.cc/t/how-do-you-convert-a-float-to-string-solved/237090/3
  String message = "Temperature: " + decimal_to_string_float(temperature, 2) + "c & Humidity: " + decimal_to_string_float(humidity, 2) + "%";
  client.publish("/mqtt", message.c_str());

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
