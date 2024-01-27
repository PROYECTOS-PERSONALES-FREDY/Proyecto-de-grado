//https://www.youtube.com/watch?v=5rHWeV0dwxo&ab_channel=FusionAutomate

/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/
/*
  Acceso con huella dactilar

  ESP32    SENSOR   
  3.3V      V+
  17        TX
  16        RX
  GND       GND

  https://forum.arduino.cc/t/conexion-entre-esp32-cam-y-huella-dactilar-dy50-del-chip-as608/1149786 codigo
  https://www.youtube.com/watch?v=Eme50HduPb8&ab_channel=JadsaTech comunicacion serial

 * There are three serial ports on the ESP known as U0UXD, U1UXD and U2UXD.
 * 
 * U0UXD is used to communicate with the ESP32 for programming and during reset/boot.
 * U1UXD is unused and can be used for your projects. Some boards use this port for SPI Flash access though
 * U2UXD is unused and can be used for your projects.
 * 
*/

// Libreria  para el Sensor de huella dactilar
#include <Adafruit_Fingerprint.h>
//Libreria para comunicacion serial
#include <HardwareSerial.h>

#include <WiFi.h>            /* WiFi library for ESP32 */
#include <Wire.h>
#include <PubSubClient.h>
#include "DHT.h"             /* DHT11 sensor library */


#define DHTPIN 4
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define wifi_ssid "FredyPC"
#define wifi_password "iotprueba"
//#define mqtt_server "192.168.0.100"
#define mqtt_server "192.168.221.100"


#define humidity_topic "sensor/DHT11/humidity"
#define temperature_celsius_topic "sensor/DHT11/temperature_celsius"
#define temperature_fahrenheit_topic "sensor/DHT11/temperature_fahrenheit"
#define fingerprint_password_topic "sensor/huella/contraseña"
#define fingerprint_submenu_topic "sensor/huella/submenu"

WiFiClient espClient;
PubSubClient client(espClient);

HardwareSerial SerialPort(2);

const uint8_t rx2Pin = 16;
const uint8_t tx2Pin = 17;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&SerialPort);

uint16_t count = 0;
uint8_t id; 
bool configuracion=false;
int submenu=0;


void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  finger.begin(57600);
  delay(5);
}

void loop() {
  getFingerprintID();
  delay(1000);
  //delay(10000); para adafruit io
  sensor_DHT11();
  while(configuracion==true){
    delay(100);
    while(submenu==1){
      configuracion();
    }
    while(submenu==2){
      configuracion();
    }
    while(submenu==3){
      configuracion();
    }
    //Serial.print("Configuracion...");
  }
}   

void configuracion(){
  if (WiFi.status() != WL_CONNECTED){
    setup_wifi();
  }

  if (!client.connected()) {
    reconnect();
  }
  client.subscribe(fingerprint_password_topic);
  client.loop();
}
void sensor_DHT11(){
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

  Serial.print("\nCelsius:");
  Serial.println(String(t).c_str());
  client.publish(temperature_celsius_topic, String(t).c_str(), true);

  Serial.print("Fahrenheit:");
  Serial.println(String(f).c_str());
  client.publish(temperature_fahrenheit_topic, String(f).c_str(), true);

  Serial.print("Humidity: ");
  Serial.println(String(h).c_str());
  Serial.print("\n");
  client.publish(humidity_topic, String(h).c_str(), true);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//------Codigo MQTT----
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
    
  if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  /*for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }*/
  Serial.println((char)payload[0]);
  if (((char)payload[0])=='*'){
    configuracion=true;
  }
  else if (((char)payload[0])=='1'){
    submenu=1;
  }
  else if (((char)payload[0])=='2'){
    submenu=2;
  }
  else if (((char)payload[0])=='3'){
    submenu=3;
  }
}

//--------Codigo Huella--------
void inicializar_huella(){
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen tomada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No se ha detectado la huella");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicacion");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Error, falla en la imagen");
      return p;
    default:
      Serial.println("Error Desconocido");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
