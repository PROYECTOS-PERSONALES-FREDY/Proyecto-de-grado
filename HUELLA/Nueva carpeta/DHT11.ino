//https://www.youtube.com/watch?v=5rHWeV0dwxo&t=157s&ab_channel=FusionAutomate
#include <WiFi.h>        
#include <Wire.h>
#include "DHT.h"             /* DHT11 sensor library */

#define DHTPIN 4
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#include "config.h"

void setup() {
  Serial.begin(115200);
  // dht.begin();
  wifiInit();
}

void wifiInit() {
    Serial.print("Conectándose a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
        delay(500);  
    }
    Serial.println("");
    Serial.println("Conectado a WiFi");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

void loop() {
  
      delay(2000);
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      float f = dht.readTemperature(true);
      
      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
      }
      
      // Compute heat index in Fahrenheit (the default)
      float hif = dht.computeHeatIndex(f, h);
      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false);
      Serial.print("\n");
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t\n");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" *C ");
      Serial.print(f);
      Serial.print(" *F\t\n");
      Serial.print("Heat index: ");
      Serial.print(hic);
      Serial.print(" *C ");
      Serial.print(hif);
      Serial.println(" *F");
      Serial.print(" \n");

      Serial.print("Humidity: ");
      Serial.println(String(h).c_str());

      Serial.print("Temperature in Celsius: ");
      Serial.println(String(t).c_str());

      Serial.print("Temperature in Fahrenheit: ");
      Serial.println(String(f).c_str());
      
}