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

#include <WiFi.h> /* WiFi library for ESP32 */
#include <Wire.h>
#include <PubSubClient.h>
#include "DHT.h" /* DHT11 sensor library */

#define DHTPIN 4
#define DHTTYPE DHT11  // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define wifi_ssid "FredyPC"
#define wifi_password "iotprueba"
//#define mqtt_server "192.168.0.25"
#define mqtt_server "10.83.83.25"

#define humidity_topic "sensor/DHT11/humidity"
#define temperature_celsius_topic "sensor/DHT11/temperature_celsius"
#define temperature_fahrenheit_topic "sensor/DHT11/temperature_fahrenheit"
#define fingerprint_start_topic "sensor/huella/inicio"
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
bool configuracion = false;
int submenu = 0;
int auxHuella1 = 0;
int auxHuella2 = 0;
int IDhuella = 0;
int auxEliminar = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  config_ini();
  finger.begin(57600);
  inicializar_huella();
  delay(5);
}

void loop() {
  config_ini();
  recepcion_datos();
  client.subscribe(fingerprint_password_topic);
  client.subscribe(fingerprint_submenu_topic);
  getFingerprintID();
  sensor_DHT11();
  delay(1000);
  //delay(10000); para adafruit io

  while (configuracion == true) {
    Serial.println("menu principal");
    recepcion_datos();
    id = 0;
    while (submenu == 1) {
      config_ini();
      numero_huellas();
      registrar_huella();
      recepcion_datos();
    }
    while (submenu == 2) {
      config_ini();
      eliminar_huella();
      numero_huellas();
      recepcion_datos();
    }
    while (submenu == 3) {
      config_ini();
      eliminar_base_datos();
      numero_huellas();
      recepcion_datos();
    }
    //Serial.print("Configuracion...");
  }
  config_ini();
  recepcion_datos();
}

void callback(char* topic, byte* payload, unsigned int length) {

  /*Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }*/
  leer_submenu((char)payload[0],(char)payload[1],(char)payload[2],topic);
}

void leer_submenu(char dato1,char dato2,char dato3, String tema){
/*
  Serial.println("\nDato actual recibido: ");
  Serial.println(dato1);
  Serial.println("\nDato anterior recibido: ");
  Serial.println(dato2);
  Serial.println("\nDato anterior al anterior recibido: ");
  Serial.println(dato3);
*/
  if (dato1=='*'&&tema=="sensor/huella/contraseña") {
    configuracion = true;
    Serial.println("\nMenu ajustes de huella");
    if (configuracion == true){client.publish(fingerprint_submenu_topic, String("!0").c_str(), true);}
  }

  else if (configuracion==true && submenu!= 1 && submenu!= 2 && submenu!= 3 && dato1=='1' && tema=="sensor/huella/contraseña") {
    Serial.println("\n\nInscripción del sensor de huellas dactilares de Adafruit");
    submenu = 1;
    dato1='0';
    dato2='0';
    dato3='0';
    client.publish(fingerprint_submenu_topic, String("!1").c_str(), true);
  } else if (configuracion==true && submenu!= 1 && submenu!= 2 && submenu!= 3 && dato1=='2' && tema=="sensor/huella/contraseña") {
    Serial.println("\n\nEliminar huellas del sensor de huellas dactilares de Adafruit");
    submenu = 2;
    dato1='0';
    dato2='0';
    dato3='0';
    client.publish(fingerprint_submenu_topic, String("!2").c_str(), true);
  } else if (configuracion==true && submenu!= 1 && submenu!= 2 && submenu!= 3 && dato1=='3' && tema=="sensor/huella/contraseña") {
    submenu = 3;
    dato1='0';
    dato2='0';
    dato3='0';
    client.publish(fingerprint_submenu_topic, String("!3").c_str(), true);
  } 
  
  else if (submenu==1 || submenu==2 && tema=="sensor/huella/contraseña") {
      if (dato1=='#' && dato2=='#'){
        IDhuella = dato3 - '0';
        id = IDhuella;
        Serial.println("\nMenu 1 ");
        Serial.print(IDhuella);
      }
      else if (dato1=='#'&& dato2!='#') {
        auxHuella1 = dato3 - '0';
        auxHuella2 = dato2 - '0';
        
        IDhuella = auxHuella1 + auxHuella2*10;
        id = IDhuella;
        Serial.println("\nMenu 2 ");
        Serial.print(IDhuella);
      }
      else if (dato1=='$'){
        Serial.println("\nReturn ");
        ESP.restart();
      }
      if (id <= 0 && id > 99) {
        return;
      }
      delay(5000);
  }
  else if (submenu==3 && tema=="sensor/huella/contraseña"){
    if(dato1 == '?'){
      Serial.println("\nELIMINANDO...");
      auxEliminar = 1;
    }
  }

}

void config_ini() {
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }

  if (!client.connected()) {
    reconnect();
  }
}

void recepcion_datos() {
  client.setCallback(callback);
  client.loop();
}

void sensor_DHT11() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  // Revisar si el sensor esta conectado
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("No se puede leer el sensor");
    return;
  }

  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("\nCelsius:");
  Serial.println(String(t).c_str());
  client.publish(temperature_celsius_topic, String(t).c_str(), true);

  Serial.print("Fahrenheit:");
  Serial.println(String(f).c_str());
  client.publish(temperature_fahrenheit_topic, String(f).c_str(), true);

  Serial.print("Humedad: ");
  Serial.println(String(h).c_str());
  Serial.print("\n");
  client.publish(humidity_topic, String(h).c_str(), true);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("direccion IP: ");
  Serial.println(WiFi.localIP());
}

//------Codigo MQTT----
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Intentando conexion MQTT...");

    if (client.connect("ESP32Client")) {
      Serial.println("connectado");
    } else {
      Serial.print("falla, rc=");
      Serial.print(client.state());
      Serial.println(" intentando en 5 segundos");
      delay(5000);
    }
  }
}

//-----------------------------
//--------Codigo Huella--------
//-----------------------------

void inicializar_huella() {
  // En este codigo se da a conocer si se detectó o no, el sensor.
  while (!finger.verifyPassword()) {
    if (finger.verifyPassword()) {                         // Realiza la verificacion: Si el sensor está activo y respondiendo, hace lo siguiente:
      Serial.println("Se encontro el sensor de huellas");  // Muestra el siguiente mensaje en el monitor serial.
      client.publish(fingerprint_start_topic, String("Se encontro el sensor de huellas").c_str(), true);
    } else {
      Serial.println("No se encontro el sensor de huellas");  // En caso contrario hace lo siguiente:
      client.publish(fingerprint_start_topic, String("No se encontro el sensor de huellas").c_str(), true);
    }
  }
  //Imprime los parametros de lectura
  Serial.println(F("Leyendo parametros del sensor"));
  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);

  finger.getTemplateCount();
}

void numero_huellas() {
  if (finger.templateCount == 0) {
    Serial.print("Sensor sin huellas registradas");
    client.publish(fingerprint_start_topic, String("Sensor sin huellas registradas").c_str(), true);
  } else {
    Serial.println("Esperando un dedo valido...");
    Serial.print("El sensor contiene ");
    Serial.print(finger.templateCount);
    Serial.println(" huellas registradas");
  }
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  recepcion_datos();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen tomada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No se ha detectado ninguna huella");
      client.publish(fingerprint_start_topic, String("No se ha detectado ninguna huella").c_str(), true);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicacion");
      client.publish(fingerprint_start_topic, String("Error de comunicacion").c_str(), true);
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Error, falla en la imagen");
      client.publish(fingerprint_start_topic, String("Error, falla en la imagen").c_str(), true);
      return p;
    default:
      Serial.println("Error Desconocido");
      client.publish(fingerprint_start_topic, String("Error Desconocido").c_str(), true);
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagen demasiado desordenada");
      client.publish(fingerprint_start_topic, String("Imagen demasiado desordenada").c_str(), true);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicacion");
      client.publish(fingerprint_start_topic, String("Error de comunicacion").c_str(), true);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("No se encontraron las caracteristicas de la huella");
      client.publish(fingerprint_start_topic, String("No se encontraron las caracteristicas de la huella").c_str(), true);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("No se encontraron las caracteristicas de la huella");
      client.publish(fingerprint_start_topic, String("No se encontraron las caracteristicas de la huella").c_str(), true);
      return p;
    default:
      Serial.println("Error desconocido");
      client.publish(fingerprint_start_topic, String("Error desconocido").c_str(), true);
      return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Coincidencia entrontrada");
    client.publish(fingerprint_start_topic, String("Encontrada coincidencia con ID# ").c_str(), true);
    client.publish(fingerprint_start_topic, String(finger.fingerID).c_str(), true);
    client.publish(fingerprint_start_topic, String(" y una certeza del ").c_str(), true);
    client.publish(fingerprint_start_topic, String(finger.confidence).c_str(), true);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicacion");
    client.publish(fingerprint_start_topic, String("Error de comunicacion").c_str(), true);
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("No se encontro ninguna coincidencia");
    client.publish(fingerprint_start_topic, String("No se encontro ninguna coincidencia").c_str(), true);
    return p;
  } else {
    Serial.println("Error desconocido");
    client.publish(fingerprint_start_topic, String("Error desconocido").c_str(), true);
    return p;
  }

  // found a match!
  Serial.print("Encontrada ID #");
  Serial.print(finger.fingerID);
  Serial.print(" con una certeza del");
  Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  // found a match!
  Serial.print("Encontrada ID #");
  Serial.print(finger.fingerID);
  Serial.print(" con una certeza del");
  Serial.println(finger.confidence);
  return finger.fingerID;
}

//----------Agregar Huella-----------

void registrar_huella() {
  Serial.println("Listo para inscribir una huella digital!");
  Serial.println("Escriba el número de identificación (del 1 al 127) en el que desea guardar este dedo...");
  client.publish(fingerprint_start_topic, String("Escriba el número de identificación (del 1 al 127) en el que desea guardar este dedo...").c_str(), true);
  //id = readnumber();
  while (id == 0) {  
    recepcion_datos();
  }
  Serial.print("inscribiendo ID #");  // Imprime el  mensaje en el monitor serial.
  Serial.println(id);

  while (!getFingerprintEnroll())
    ;  // Mientras (obtener registro de huella) devuelve falso, seguirá ejecutandose. (LOGICA NEGATIVA)
       // En caso de (obtener registro de huella) devuelva verdadero, finalizará el bucle.
}

uint8_t getFingerprintEnroll() {  // La funcion para obtener o realizar una huella digital como un entero sin signo de bits.

  int p = -1;  // Se declara una variable local de tipo entero con el nombre "p", igual al valor de -1
  Serial.print("Esperando un dedo válido para inscribir como #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {           // Inicia la iteracion: mientras p es distinto a huella_digital_ok, entonces hace lo siguiente:
    p = finger.getImage();                // Se le asigna a la variable p, la obtencion de la imagen.
    switch (p) {                          // Seleccionar p
      case FINGERPRINT_OK:                // En caso de; huella digital correcta, hace lo siguiente:
        Serial.println("Imagen tomada");  // Imprime el  mensaje en el monitor serial.
        client.publish(fingerprint_start_topic, String("Imagen tomada").c_str(), true);
        break;                                    // Realiza el  corte.
      case FINGERPRINT_NOFINGER:                  // En caso de; sin huella dactilar, hace lo siguiente:
        Serial.println(".");                      // Imprime el  mesaje en el monitor serial.
        break;                                    // Realiza el  corte.
      case FINGERPRINT_PACKETRECIEVEERR:          // En caso de; error al recibir el paquete de huella dactilar, hace lo siguiente:
        Serial.println("Error de comunicación");  // Imprime el  mensaje en el monitor serial.
        break;                                    // Realiza el  corte.
      case FINGERPRINT_IMAGEFAIL:                 // En caso de; fallo de imagen de la huella dactilar, hace lo siguiente:
        Serial.println("Error de imagen");
        client.publish(fingerprint_start_topic, String("Error de imagen").c_str(), true);  // Imprime el  mensaje en el monitor serial.
        break;                                                                             // Realiza el  corte.
      default:                                                                             // En caso de; que no haya coincidencia con ninguno de los casos anteriores, hace lo siguiente:
        Serial.println("Error desconocido");                                               // Imprime el  mensaje en el monitor serial.
        break;                                                                             // Realiza el  corte.
    }
  }
  /*  Si se establecio el registro del dedo, se pasa a la siguiente instruccion:
  //  Plantilla: Extraccion de la huella; islas, crestas, valles, bifurcacion, nucleos.
      Se covierte la primera imagen de la huella dactilar*/
  p = finger.image2Tz(1);  // Convierte la imagen de la huella en una plantilla y almacena el resultado en la variable.
  switch (p) {
    case FINGERPRINT_OK:                    // En caso de; huella digital correcta, hace lo siguiente:
      Serial.println("Imagen convertida");  // Imprime el  mensaje en el monitor serial.
      client.publish(fingerprint_start_topic, String("Imagen convertida").c_str(), true);
      break;                                           // Realiza el  corte.
    case FINGERPRINT_IMAGEMESS:                        // En caso de; imagen de huella confusa, hace lo siguiente:
      Serial.println("Imagen demasiado desordenada");  // Imprime el  mensaje en el monitor serial.
      client.publish(fingerprint_start_topic, String("Imagen demasiado desordenada").c_str(), true);
      return p;                                 // Retorna en  p
    case FINGERPRINT_PACKETRECIEVEERR:          // En caso de; error al recibir el paquete de huella dactilar, hace lo siguiente:
      Serial.println("Error de comunicación");  // Imprime el  mensaje en el monitor serial.
      client.publish(fingerprint_start_topic, String("Error de comunicación").c_str(), true);
      return p;                                                                             // Retorna en  p
    case FINGERPRINT_FEATUREFAIL:                                                           // En caso de; fallo de caracteristica de huella, hace lo siguiente
      Serial.println("No se pudieron encontrar las características de la huella digital");  // Imprime el  mensaje en el monitor serial.
      return p;                                                                             // Retorna en  p
    case FINGERPRINT_INVALIDIMAGE:                                                          // En caso de; imagen de huella digital no validad, hace lo siguiente
      Serial.println("No se pudieron encontrar las características de la huella digital");  // Imprime el  mensaje en el monitor serial.
      return p;                                                                             // Retorna en  p
    default:                                                                                // En caso de; que no haya coincidencia con ninguno de los casos anteriores, hace lo siguiente:
      Serial.println("Error desconocido");                                                  // Imprime el  mensaje en el monitor serial.
      return p;                                                                             // Retorna en  p
  }
  // Si se establecio la conversion la primera imagen de la huella dactilar , se pasa a la siguiente instruccion:
  Serial.println("Quitar el dedo");  // Imprime el  mensaje en el monitor serial.
  client.publish(fingerprint_start_topic, String("Quitar el dedo").c_str(), true);
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Vuelva a colocar el mismo dedo");  // Imprime el  mensaje en el monitor serial.
  client.publish(fingerprint_start_topic, String("Vuelva a colocar el mismo dedo").c_str(), true);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Imagen tomada");
        client.publish(fingerprint_start_topic, String("Imagen tomada").c_str(), true);
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Error de comunicacion");
        client.publish(fingerprint_start_topic, String("Error de comunicación").c_str(), true);
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Error de imagen");
        client.publish(fingerprint_start_topic, String("Error de imagen").c_str(), true);
        break;
      default:
        Serial.println("Error desconocido");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen convertida");
      client.publish(fingerprint_start_topic, String("Imagen convertida").c_str(), true);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagen demasiado desordenada");
      client.publish(fingerprint_start_topic, String("Imagen demasiado desordenada").c_str(), true);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicacion");
      client.publish(fingerprint_start_topic, String("Error de comunicacion").c_str(), true);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("No se pudieron encontrar las características de la huella digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("No se pudieron encontrar las características de la huella digital");
      return p;
    default:
      Serial.println("Error desconocido");
      return p;
  }

  // OK converted!
  Serial.print("Creando modelo para #");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("¡Estampados combinados!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicacion");
    client.publish(fingerprint_start_topic, String("Error de comunicacion").c_str(), true);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Las huellas dactilares no coinciden");
    client.publish(fingerprint_start_topic, String("Las huellas dactilares no coinciden").c_str(), true);
    return p;
  } else {
    Serial.println("Error desconocido");
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("¡Almacenado!");
    client.publish(fingerprint_start_topic, String("Huella registrada").c_str(), true);
    submenu = 0;
    configuracion = false;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicacion");
    client.publish(fingerprint_start_topic, String("Error de comunicacion").c_str(), true);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("No se pudo almacenar en esa ubicación");
    client.publish(fingerprint_start_topic, String("No se pudo almacenar en esa ubicación").c_str(), true);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error al escribir en la memoria interna");
    client.publish(fingerprint_start_topic, String("Error al escribir en la memoria interna").c_str(), true);
    return p;
  } else {
    Serial.println("Error desconocido");
    return p;
  }
  return true;
}

//-------------Eliminar huella--------

void eliminar_huella() {
  while (id == 0) {  
    recepcion_datos();
  }
  Serial.print("Eliminando huella con ID #");
  Serial.println(id);

  deleteFingerprint(id);
  id = 0;
  submenu = 0;
  delay(5000);
}


uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Eliminada");
    submenu = 0;
    configuracion = false;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicacion");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Error ID");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error rescribiendo la memoria interna");
  } else {
    Serial.print("Error desconocido: 0x");
    Serial.println(p, HEX);
  }

  return p;
}

//-------------Eliminar base de datos-------

void eliminar_base_datos() {
  Serial.println("\n\nEliminando todas las huellas");
  Serial.println("Presiona 'Y' para continuar");
  while (auxEliminar == 0) {
    recepcion_datos();
    /*
    if (Serial.available() && (Serial.read() == 'Y')) {
      break;
    }*/
  }
  while (!finger.verifyPassword()) {
    if (finger.verifyPassword()) {                         // Realiza la verificacion: Si el sensor está activo y respondiendo, hace lo siguiente:
      Serial.println("Se encontro el sensor de huellas");  // Muestra el siguiente mensaje en el monitor serial.
    } else {
      Serial.println("No se encontro el sensor de huellas");  // En caso contrario hace lo siguiente:
    }
  }

  finger.emptyDatabase();
  Serial.println("Ahora la base de datos esta borrada...");
  submenu = 0;
  configuracion = false;
  auxEliminar = 0;
}