//https://www.youtube.com/watch?v=a-YILV2ziKs&ab_channel=ChepeCarlos

#include <WiFi.h>
#include <ESPmDNS.h>
#include <TelnetStream.h>

#include "data.h"

int Contador = 0;
unsigned long anterior = 0;

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("Iniciando ESP...");
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Error con Wifi, reiniciando...");
    delay(5000);
    ESP.restart();
  }

  MDNS.begin(nombre);
  MDNS.addService("telnet", "tcp", 23);

  Serial.printf("Listo!\nConectate a %s.local con Telnet\n", nombre);
  Serial.print("o en la IP: ");
  Serial.println(WiFi.localIP());
  TelnetStream.begin(); // inicializar protocolo TELNET
}

void loop() {
  LeerTelnet();
  EnviarTelnet();
  delay(2);
}

void LeerTelnet() {
  if (TelnetStream.available()) {
    char Letra = TelnetStream.read();
    switch (Letra) {
      case 'R':
        TelnetStream.stop();
        delay(100);
        ESP.restart();
        break;
      case 'C':
        TelnetStream.println("Adios, adios");
        TelnetStream.stop();
    }
  }
}

void EnviarTelnet() {
  if (millis() - anterior > 5000) {
    anterior = millis();
    TelnetStream.print(millis() / 1000);
    TelnetStream.print(" Segundos, Contador=");
    TelnetStream.println(Contador++);
  }
}
