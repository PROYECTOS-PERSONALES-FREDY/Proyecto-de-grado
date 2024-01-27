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

 
DS1302

// CONNECTIONS:
// DS1302 I2C_SDA - DAT --> 21
// DS1302 I2C_SCL - CLK --> 22
// DS1302 RST/CE --> 0
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

*/
//Importa varias librerías necesarias para el funcionamiento de los distintos componentes, como el sensor de huella dactilar, sensor DHT11, módulo WiFi, RTC (Real-Time Clock), etc.

// Libreria  para el Sensor de huella dactilar
#include <Adafruit_Fingerprint.h>
//Libreria para comunicacion serial
#include <HardwareSerial.h>
#include <RtcDS1302.h>

#include <WiFi.h> /* WiFi library for ESP32 */
#include <Wire.h>
#include <PubSubClient.h>
#include "DHT.h" /* DHT11 sensor library */

#define DHTPIN 4
#define DHTTYPE DHT11  // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define countof(a) (sizeof(a) / sizeof(a[0])) //fecha y hora

#define wifi_ssid "Carolina"
#define wifi_password "JK8519Min"
#define mqtt_server "192.168.1.25"
//#define mqtt_server "10.83.83.25"

#define humidity_topic "sensor/DHT11/humidity"
#define temperature_celsius_topic "sensor/DHT11/temperature_celsius"
#define temperature_fahrenheit_topic "sensor/DHT11/temperature_fahrenheit"
#define fingerprint_start_topic "sensor/huella/inicio"
#define fingerprint_password_topic "sensor/huella/contraseña"
#define fingerprint_submenu_topic "sensor/huella/submenu"

//Declara instancias de sensores, módulos de comunicación, clientes MQTT y variables de control que se utilizan a lo largo del código.

ThreeWire myWire(21,22,0); // SDA - DAT, SCL - CLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

WiFiClient espClient;
PubSubClient client(espClient);

HardwareSerial SerialPort(2);

const uint8_t rx2Pin = 16;
const uint8_t tx2Pin = 17;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&SerialPort);

int electroiman = 25;

uint16_t count = 0;
uint8_t id;
bool configuracion = false;
int submenu = 0;
int auxHuella1 = 0;
int auxHuella2 = 0;
int IDhuella = 0;
int auxEliminar = 0;
String dato_fecha_hora = "";
void setup() {
  /**
 * Configuración inicial al iniciar el dispositivo.
 * Inicia la comunicación serial a una velocidad de 115200 baudios.
 * Configura la conexión WiFi.
 * Inicia el sensor DHT11 para la lectura de temperatura y humedad.
 * Configura la fecha y hora del dispositivo.
 * Establece el servidor MQTT y configura el cliente MQTT para conectarse al servidor.
 * Configura las funciones de callback y de configuración inicial.
 * Inicia el sensor de huellas digitales.
 * Establece un pequeño retraso y configura un pin como salida.
 */

  Serial.begin(115200);
  setup_wifi();
  dht.begin();
  fecha_hora();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  config_ini();
  finger.begin(57600);
  inicializar_huella();
  delay(5);
  pinMode(electroiman,OUTPUT);
  client.subscribe(fingerprint_password_topic);
  client.subscribe(fingerprint_submenu_topic);
  client.publish(fingerprint_submenu_topic, String("!!").c_str(), true);
}

void loop() {
  /**
 * Realizar ciclos de verificación, suscripción a temas MQTT y ejecución de acciones basadas en comandos MQTT.
 * Verifica y gestiona las configuraciones iniciales.
 * Se suscribe a temas específicos en el servidor MQTT para recibir instrucciones.
 * Realiza la detección de huellas dactilares, lecturas del sensor DHT11 y publica datos en los temas MQTT correspondientes.
 * Ejecuta diferentes configuraciones basadas en comandos recibidos a través de MQTT.
 * Dentro de un bucle while, gestiona el menú principal dependiendo de la configuración y submenús.
 * Realiza operaciones como registrar huellas, eliminar huellas o eliminar toda la base de datos de huellas según las selecciones del menú.
 * Reinicia y espera nuevas instrucciones y configuraciones.
 */

  config_ini();
  recepcion_datos();
  getFingerprintID();
  sensor_DHT11();
  imprimir_fecha_hora();
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

/**
 * Función de devolución de llamada para el cliente MQTT.
 * Esta función recibe los datos del mensaje MQTT y los pasa a la función 'leer_submenu' para su procesamiento.
 * topic El tema del mensaje MQTT.
 * payload Los datos del mensaje MQTT como arreglo de bytes.
 * length La longitud de los datos del mensaje MQTT.
 */

void callback(char* topic, byte* payload, unsigned int length) {
  // Llama a la función 'leer_submenu' para procesar los datos del mensaje MQTT
  // Convierte los bytes del payload a caracteres y los pasa como argumentos a 'leer_submenu'
  leer_submenu((char)payload[0],(char)payload[1],(char)payload[2],topic);
}

void leer_submenu(char dato1,char dato2,char dato3, String tema){
  /**
 * Esta función lee los datos recibidos y ejecuta acciones dependiendo de los valores y el tema recibido.
 * dato1 El primer dato recibido.
 * dato2 El segundo dato recibido.
 * ato3 El tercer dato recibido.
 * tema El tema del mensaje MQTT recibido.
 */

  // Comprobación y ejecución de acciones según los datos y el tema recibido
  if (dato1=='$'){
        Serial.println("\nReturn ");
        ESP.restart();}
  
  // Verifica si el dato1 es '*' y el tema es "sensor/huella/contraseña"
  if (dato1=='*'&&tema=="sensor/huella/contraseña") {
    // Establece configuracion como verdadero
    configuracion = true;
    Serial.println("\nMenu ajustes de huella");
    if (configuracion == true){client.publish(fingerprint_submenu_topic, String("!0").c_str(), true);client.publish(fingerprint_submenu_topic, String("!0").c_str(), true);}
  }
  // Verifica condiciones para diferentes operaciones basadas en configuraciones y submenús
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
  /**
 * Esta función verifica la configuración inicial de la conexión WiFi y del cliente MQTT.
 * Si la conexión WiFi no está establecida, se configura.
 * Si el cliente MQTT no está conectado, se intenta restablecer la conexión.
 */
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }
  if (!client.connected()) {
    reconnect();
  }
}

void recepcion_datos() {
/**
 * Esta función se encarga de recibir los datos a través del cliente MQTT y llama a la función de devolución de llamada.
 */
  client.setCallback(callback);
  client.loop();
}

void sensor_DHT11() {
/**
 * Esta función se encarga de leer un sensor DHT11 para obtener lecturas de temperatura y humedad. Luego, publica estos datos en dos topics MQTT distintos para su uso o visualización externa.
 * float h = dht.readHumidity();: Lee la humedad del sensor DHT11 y la almacena en la variable h.
 * float t = dht.readTemperature();: Lee la temperatura en grados Celsius del sensor DHT11 y la almacena en la variable t.
 * float f = dht.readTemperature(true);: Lee la temperatura en grados Fahrenheit del sensor DHT11 y la almacena en la variable f.
 * Se definen tres cadenas de texto: envio_celsius, envio_humedad, y separador para almacenar los datos de temperatura y humedad junto con un separador ;.
 * Verificación de conexión del sensor: Si alguno de los valores leídos (h, t, f) es inválido (isnan), imprime un mensaje de error por el puerto serie y finaliza la ejecución de la función.
 * Cálculo del índice de calor (heat index) tanto en Fahrenheit (hif) como en Celsius (hic) utilizando la librería del sensor DHT11.
 * Publicación de datos:
 * Temperatura: Imprime la temperatura en grados Celsius por el puerto serie junto con un sello de tiempo (dato_fecha_hora) y la publica en el topic MQTT temperature_celsius_topic.
 * Humedad: Imprime el valor de humedad por el puerto serie junto con un sello de tiempo (dato_fecha_hora) y lo publica en el topic MQTT humidity_topic.
 */

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  String envio_celsius = "";
  String envio_humedad = "";
  String separador = ";";
  // Revisar si el sensor esta conectado
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("No se puede leer el sensor DHT11");
    return;
  }

  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("\nCelsius:");
  Serial.println(String(t).c_str());
  Serial.println(String(dato_fecha_hora).c_str());
  envio_celsius = t + separador + dato_fecha_hora;
  client.publish(temperature_celsius_topic, String(envio_celsius).c_str(), true);

  Serial.print("Humedad: ");
  Serial.println(String(h).c_str());
  envio_humedad = h + separador + dato_fecha_hora;
  client.publish(humidity_topic, String(envio_humedad).c_str(), true);

}

void setup_wifi() {
  /**
 * Esta función se encarga de establecer la conexión WiFi del ESP32 utilizando las credenciales proporcionadas (SSID y contraseña). Muestra el progreso de la conexión por el puerto serie y finalmente imprime la dirección IP asignada al ESP32 una vez que se ha conectado a la red WiFi.
 * delay(10);: Realiza un pequeño retraso inicial.
 * Muestra un mensaje indicando la red WiFi a la que se va a conectar: Conectando a [wifi_ssid] (donde wifi_ssid es el nombre de la red).
 * Inicia la conexión a la red WiFi utilizando WiFi.begin(wifi_ssid, wifi_password), donde wifi_ssid es el nombre de la red y wifi_password es la contraseña.
 * Espera hasta que se establezca la conexión a la red WiFi mediante un bucle while:
 * Dentro del bucle, se utiliza WiFi.status() para verificar continuamente el estado de la conexión.
 * Muestra un punto en el puerto serie (Serial.print(".")) cada 500 milisegundos mientras espera la conexión.
 * Una vez que se ha establecido la conexión (WL_CONNECTED), imprime por el puerto serie:
 * Un mensaje indicando que se ha conectado a la red WiFi (WiFi conectado).
 * La dirección IP asignada al ESP32 (direccion IP:) seguido de la IP obtenida (WiFi.localIP()).
 */

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
  // Bucle mientras se conecta
  while (!client.connected()) { // Mientras no estemos conectados al cliente MQTT
    Serial.print("Intentando conexion MQTT..."); // Imprime un mensaje indicando el intento de conexión

    if (client.connect("ESP32Client")) { // Intenta conectar al cliente MQTT con el nombre "ESP32Client"
      Serial.println("connectado"); // Si la conexión tiene éxito, imprime "conectado"
    } else {
      Serial.print("falla, rc="); // Si la conexión falla, imprime el código de estado de la conexión
      Serial.print(client.state()); // Imprime el estado actual del cliente MQTT
      Serial.println(" intentando en 5 segundos"); // Indica que se intentará nuevamente en 5 segundos
      delay(5000); // Espera 5 segundos antes de intentar nuevamente la conexión
    }
  }
}
//-----------------------------------
//--------Codigo Fecha y hora--------
//-----------------------------------
void fecha_hora(){
  Rtc.Begin(); // Inicializa el objeto RTC (Real-Time Clock)

  // Imprime la fecha y hora en que se compiló el código
  Serial.print("compilado: ");
  Serial.print(__DATE__); // Macro que contiene la fecha de compilación
  Serial.println(__TIME__); // Macro que contiene la hora de compilación

  // Crea un objeto RtcDateTime con la fecha y hora de compilación
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  // Imprime la fecha y hora de compilación
  printDateTime(compiled);
  Serial.println();

  // Verifica si la fecha y hora del RTC son válidas, de lo contrario, las establece con la fecha y hora de compilación
  if (!Rtc.IsDateTimeValid()) {
      Serial.println("Fecha y hora inválidas del sensor");
      Rtc.SetDateTime(compiled);
  }

  // Verifica si el RTC está protegido contra escritura y lo deshabilita si es así
  if (Rtc.GetIsWriteProtected()) {
      Serial.println("El sensor tiene la escritura protegida, habilitando escritura...");
      Rtc.SetIsWriteProtected(false);
  }

  // Verifica si el RTC no está activo y lo inicia
  if (!Rtc.GetIsRunning()) {
      Serial.println("RTC no está activado, iniciando...");
      Rtc.SetIsRunning(true);
  }

  // Obtiene la fecha y hora actual del RTC
  RtcDateTime now = Rtc.GetDateTime();

  // Compara la fecha y hora actual con la fecha y hora de compilación
  if (now < compiled) {
      Serial.println("Actualizando fecha del sensor...");
      Rtc.SetDateTime(compiled); // Actualiza la fecha y hora del RTC con la de compilación
  }
  else if (now > compiled) {
      Serial.println("Fecha guardada del sensor");
  }
  else if (now == compiled) {
      Serial.println("Fecha actualizada");
  }
}
void imprimir_fecha_hora(){
  RtcDateTime now = Rtc.GetDateTime();

  printDateTime(now);
  Serial.println();

  if (!now.IsValid())
  {
      Serial.println("No conectado...");
  }
}

void printDateTime(const RtcDateTime& dt)
{
    // Se crea un array de caracteres para almacenar la fecha y hora formateada
    char datestring[26];

    // Se utiliza la función snprintf_P para formatear la fecha y hora en el array de caracteres
    snprintf_P(datestring, 
            countof(datestring), // countof() es una macro que devuelve el tamaño del array
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"), // Formato de fecha y hora
            dt.Day(), // Obtiene el día del objeto RtcDateTime
            dt.Month(), // Obtiene el mes del objeto RtcDateTime
            dt.Year(), // Obtiene el año del objeto RtcDateTime
            dt.Hour(), // Obtiene la hora del objeto RtcDateTime
            dt.Minute(), // Obtiene los minutos del objeto RtcDateTime
            dt.Second() ); // Obtiene los segundos del objeto RtcDateTime

    // Se asigna la cadena formateada a la variable dato_fecha_hora
    dato_fecha_hora = datestring;
}

//-----------------------------
//--------Codigo Huella--------
//-----------------------------
void inicializar_huella() {
  // Este bucle verifica si se puede acceder al sensor de huellas mediante la contraseña
  while (!finger.verifyPassword()) {
    if (finger.verifyPassword()) {
      // Si la verificación tiene éxito, muestra un mensaje de que se encontró el sensor de huellas
      Serial.println("Se encontró el sensor de huellas");
      client.publish(fingerprint_start_topic, String("Se encontró el sensor de huellas").c_str(), true);
    } else {
      // Si la verificación falla, muestra un mensaje indicando que no se encontró el sensor
      Serial.println("No se encontró el sensor de huellas");
      client.publish(fingerprint_start_topic, String("No se encontró el sensor de huellas").c_str(), true);
    }
  }

  // Imprime los parámetros del sensor una vez que se ha verificado la contraseña
  Serial.println(F("Leyendo parámetros del sensor"));
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

  // Obtiene el número de huellas almacenadas en el sensor
  finger.getTemplateCount();
}

void numero_huellas() {
  // Verifica si no hay huellas registradas en el sensor
  if (finger.templateCount == 0) {
    Serial.print("Sensor sin huellas registradas");
    client.publish(fingerprint_start_topic, String("Sensor sin huellas registradas").c_str(), true);
  } else {
    // Si hay huellas registradas, muestra un mensaje indicando cuántas huellas hay
    Serial.println("Esperando un dedo válido...");
    Serial.print("El sensor contiene ");
    Serial.print(finger.templateCount);
    Serial.println(" huellas registradas");
  }
}


uint8_t readnumber(void) {
  uint8_t num = 0; // Inicializa una variable 'num' de tipo uint8_t (entero sin signo de 8 bits) y la establece en 0.

  while (num == 0) { // Inicia un bucle que continúa mientras 'num' sea igual a 0.
    while (!Serial.available()) // Espera hasta que haya datos disponibles en el puerto serial.
      ; // Espera activa hasta que haya datos disponibles.

    num = Serial.parseInt(); // Lee el número entero disponible en el puerto serial y lo almacena en 'num'.
  }

  return num; // Devuelve el número leído desde el puerto serial.
}


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage(); // Obtiene una imagen de la huella dactilar
  recepcion_datos(); // Posible función de manejo de datos

  // Realiza diferentes acciones según el resultado de la obtención de la imagen
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen tomada");
      // Se informa que se ha capturado la imagen correctamente
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No se ha detectado ninguna huella");
      // Publica un mensaje indicando que no se ha detectado ninguna huella
      client.publish(fingerprint_start_topic, String("No se ha detectado ninguna huella").c_str(), true);
      return p; // Retorna el estado de no detección de huella
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicacion");
      // Publica un mensaje indicando un error de comunicación
      client.publish(fingerprint_start_topic, String("Error de comunicacion").c_str(), true);
      return p; // Retorna el estado de error de comunicación
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Error, falla en la imagen");
      // Publica un mensaje indicando un fallo en la imagen
      client.publish(fingerprint_start_topic, String("Error, falla en la imagen").c_str(), true);
      return p; // Retorna el estado de fallo en la imagen
    default:
      Serial.println("Error Desconocido");
      // Publica un mensaje indicando un error desconocido
      client.publish(fingerprint_start_topic, String("Error Desconocido").c_str(), true);
      return p; // Retorna un estado de error desconocido
  }

  // Si no hay problemas con la captura de la imagen, continúa con la verificación y búsqueda
  p = finger.image2Tz(); // Convierte la imagen capturada en una plantilla
  switch (p) {
    // Realiza diferentes acciones según el resultado de la conversión de la imagen
    case FINGERPRINT_OK:
      Serial.println("Imagen convertida");
      // Informa que la imagen se ha convertido correctamente
      break;
    // ... (casos similares a los anteriores con distintos tipos de errores)
    default:
      Serial.println("Error desconocido");
      // Publica un mensaje indicando un error desconocido
      client.publish(fingerprint_start_topic, String("Error desconocido").c_str(), true);
      return p; // Retorna un estado de error desconocido
  }

  // Realiza una búsqueda de huella con la plantilla actual
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Coincidencia encontrada");
    // Publica un mensaje indicando que se ha encontrado una coincidencia con una huella existente
    client.publish(fingerprint_start_topic, String("Encontrada coincidencia con ID# ").c_str(), true);
    client.publish(fingerprint_start_topic, String(finger.fingerID).c_str(), true);
    client.publish(fingerprint_start_topic, String(" y una certeza del ").c_str(), true);
    client.publish(fingerprint_start_topic, String(finger.confidence).c_str(), true);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicacion");
    // Publica un mensaje indicando un error de comunicación
    client.publish(fingerprint_start_topic, String("Error de comunicacion").c_str(), true);
    return p; // Retorna el estado de error de comunicación
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("No se encontró ninguna coincidencia");
    // Publica un mensaje indicando que no se encontró ninguna coincidencia con huellas existentes
    client.publish(fingerprint_start_topic, String("No se encontró ninguna coincidencia").c_str(), true);
    return p; // Retorna el estado de no coincidencia
  } else {
    Serial.println("Error desconocido");
    // Publica un mensaje indicando un error desconocido
    client.publish(fingerprint_start_topic, String("Error desconocido").c_str(), true);
    return p; // Retorna un estado de error desconocido
  }

  // Si hay una coincidencia con una huella existente, muestra información y realiza acciones
  Serial.print("Encontrada ID #");
  Serial.print(finger.fingerID);
  Serial.print(" con una certeza del ");
  Serial.println(finger.confidence);
  // Realiza acciones como activar un electroimán durante un tiempo específico
  digitalWrite(electroiman, HIGH);
  delay(10000);
  digitalWrite(electroiman, LOW);
  return finger.fingerID; // Retorna la ID de la huella encontrada
}


// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage(); // Captura una imagen de la huella
  if (p != FINGERPRINT_OK) return -1; // Si la captura de la imagen falla, retorna -1 indicando un error

  p = finger.image2Tz(); // Convierte la imagen en una plantilla
  if (p != FINGERPRINT_OK) return -1; // Si la conversión de la imagen a plantilla falla, retorna -1 indicando un error

  p = finger.fingerFastSearch(); // Realiza una búsqueda rápida de la huella
  if (p != FINGERPRINT_OK) return -1; // Si la búsqueda rápida de la huella falla, retorna -1 indicando un error

  // Si se encuentra una coincidencia con una huella existente
  Serial.print("Encontrada ID #");
  Serial.print(finger.fingerID);
  Serial.print(" con una certeza del");
  Serial.println(finger.confidence);
  
  return finger.fingerID; // Retorna la ID de la huella encontrada
}


//----------Agregar Huella-----------
void registrar_huella() {
  Serial.println("Listo para inscribir una huella digital!"); // Muestra un mensaje de preparación en el monitor serial.
  Serial.println("Escriba el número de identificación (del 1 al 127) en el que desea guardar este dedo..."); // Solicita al usuario que ingrese un número de identificación válido.
  client.publish(fingerprint_start_topic, String("Escriba el número de identificación (del 1 al 127) en el que desea guardar este dedo...").c_str(), true); // Publica el mensaje a través del cliente MQTT.

  //id = readnumber(); // Se espera que el usuario ingrese un número mediante la función readnumber() (código comentado).

  while (id == 0) {  
    recepcion_datos(); // Posible función de recepción de datos.
  }

  Serial.print("Inscribiendo ID #");  // Imprime un mensaje indicando que se está inscribiendo una huella con cierto ID.
  Serial.println(id); // Muestra el ID de la huella que se está inscribiendo en el monitor serial.

  while (!getFingerprintEnroll())
    ; // Realiza el proceso de inscripción de la huella. Mientras la función getFingerprintEnroll() devuelva falso, continuará intentando inscribir la huella.
}

uint8_t getFingerprintEnroll() {   // Se inicializa una variable local "p" de tipo entero en -1

  int p = -1;  // Se declara una variable local de tipo entero con el nombre "p", igual al valor de -1
  Serial.print("Esperando un dedo válido para inscribir como #");
  Serial.println(id);
  // Bucle para realizar la captura de la imagen de la huella
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
  Serial.println("Retirar el dedo");  // Imprime el  mensaje en el monitor serial.
  client.publish(fingerprint_start_topic, String("Retirar el dedo").c_str(), true);
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
  // Espera a que se ingrese un ID de huella válido
  while (id == 0) {  
    recepcion_datos();
  }

  // Muestra en el monitor serial el ID de la huella que se va a eliminar
  Serial.print("Eliminando huella con ID #");
  Serial.println(id);

  // Llama a la función deleteFingerprint para eliminar la huella con el ID especificado
  deleteFingerprint(id);

  // Publica un mensaje a través del cliente MQTT indicando que la huella ha sido eliminada correctamente
  client.publish(fingerprint_start_topic, String("Huella eliminada correctamente").c_str(), true);

  // Reinicia el valor de "id" a cero y los valores de "submenu" para salir de este modo de eliminación
  id = 0;
  submenu = 0;

  // Espera durante 5 segundos antes de continuar
  delay(5000);
}

uint8_t deleteFingerprint(uint8_t id) {
  // Se inicializa "p" con un valor inválido para manejar errores
  uint8_t p = -1;

  // Se llama a la función deleteModel del sensor de huellas con el ID proporcionado para eliminar la huella correspondiente
  p = finger.deleteModel(id);

  // Se realizan diferentes acciones dependiendo del resultado de la operación de eliminación
  if (p == FINGERPRINT_OK) {
    // En caso de que la huella se elimine correctamente, muestra un mensaje en el monitor serial y actualiza algunas variables
    Serial.println("Eliminada");
    submenu = 0;
    configuracion = false;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    // Si hay un error de comunicación, muestra un mensaje en el monitor serial y publica un mensaje a través del cliente MQTT
    Serial.println("Error de comunicacion");
    client.publish(fingerprint_start_topic, String("Error de comunicacion").c_str(), true);
  } else if (p == FINGERPRINT_BADLOCATION) {
    // Si hay un error con el ID, muestra un mensaje en el monitor serial y publica un mensaje a través del cliente MQTT
    Serial.println("Error ID");
    client.publish(fingerprint_start_topic, String("Error ID").c_str(), true);
  } else if (p == FINGERPRINT_FLASHERR) {
    // Si hay un error al reescribir la memoria interna, muestra un mensaje en el monitor serial y publica un mensaje a través del cliente MQTT
    Serial.println("Error rescribiendo la memoria interna");
    client.publish(fingerprint_start_topic, String("Error rescribiendo la memoria interna").c_str(), true);
  } else {
    // Si hay un error desconocido, muestra el código de error en formato hexadecimal en el monitor serial
    Serial.print("Error desconocido: 0x");
    Serial.println(p, HEX);
  }

  // Devuelve el código de error resultante de la operación de eliminación
  return p;
}


//-------------Eliminar base de datos-------
void eliminar_base_datos() {
  // Muestra mensajes en el monitor serial para indicar el inicio del proceso de eliminación de todas las huellas
  Serial.println("\n\nEliminando todas las huellas");
  Serial.println("Presiona 'Y' para continuar");

  // Espera a que se ingrese 'Y' para confirmar la eliminación de todas las huellas
  while (auxEliminar == 0) {
    recepcion_datos(); // Maneja la recepción de datos
  }

  // Verifica la conexión con el sensor de huellas dactilares antes de proceder
  while (!finger.verifyPassword()) {
    if (finger.verifyPassword()) {                         // Realiza la verificación: Si el sensor está activo y responde, hace lo siguiente:
      Serial.println("Se encontró el sensor de huellas");  // Muestra el mensaje correspondiente en el monitor serial.
    } else {
      Serial.println("No se encontró el sensor de huellas");  // En caso contrario, muestra el mensaje correspondiente en el monitor serial.
    }
  }

  // Elimina todas las huellas almacenadas en la base de datos del sensor
  finger.emptyDatabase();

  // Muestra un mensaje en el monitor serial y publica un mensaje a través del cliente MQTT para informar que la base de datos ha sido eliminada
  Serial.println("Ahora la base de datos está borrada...");
  client.publish(fingerprint_start_topic, String("Base de datos eliminada...").c_str(), true);

  // Actualiza algunas variables para reflejar que el proceso de eliminación ha finalizado
  submenu = 0;
  configuracion = false;
  auxEliminar = 0; // Restablece el valor de la variable auxiliar para futuros usos
}
