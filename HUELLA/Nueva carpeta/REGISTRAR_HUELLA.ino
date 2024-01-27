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

HardwareSerial SerialPort(2);

const uint8_t rx2Pin = 16;
const uint8_t tx2Pin = 17;


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&SerialPort);

uint16_t count = 0;
uint8_t id; 

void setup()
{
  Serial.begin(9600); // Se configura la velocidad de 9600 baudios que va trabajar con el monitor serie
  while (!Serial);  // Inicia la iteracion: Mientras Serial es negativo, hace lo siguiente 
  delay(100);       // Se retrasa el programa con un tiempo de 100 milisegundos. 
  Serial.println("\n\nInscripción del sensor de huellas dactilares de Adafruit"); // Se imprime en el monitor serial el siguiente mensaje.

  finger.begin(57600); // Establece la comunicacion serial del sensor de huellas a 57600 baudios.

// En este codigo se da a conocer si se detectó o no, el sensor.
  while (!finger.verifyPassword()){
    if (finger.verifyPassword()) { // Realiza la verificacion: Si el sensor está activo y respondiendo, hace lo siguiente:
      Serial.println("Found fingerprint sensor!");// Muestra el siguiente mensaje en el monitor serial.
    } else {
      Serial.println("Did not find fingerprint sensor :(");// En caso contrario hace lo siguiente:
    }
  }

  //Imprime los parametros de lectura
  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}

void loop()                     // run over and over again
{
  Serial.println("Listo para inscribir una huella digital!");
  Serial.println("Escriba el número de identificación (del 1 al 127) en el que desea guardar este dedo como...");
  id = readnumber();
  if (id == 0) {                   // Realiza la verificacion: Si id es 0 retorna
     return;
  }
  Serial.print("inscribiendo ID #");    // Imprime el  mensaje en el monitor serial.
  Serial.println(id);

  while (!  getFingerprintEnroll() ); // Mientras (obtener registro de huella) devuelve falso, seguirá ejecutandose. (LOGICA NEGATIVA)
                                      // En caso de (obtener registro de huella) devuelva verdadero, finalizará el bucle.
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
// Se crea la funcion para obtener o realizar una huella digital:  
uint8_t getFingerprintEnroll() { // La funcion para obtener o realizar una huella digital como un entero sin signo de bits.

  int p = -1;                // Se declara una variable local de tipo entero con el nombre "p", igual al valor de -1
  Serial.print("Esperando un dedo válido para inscribir como #"); 
  Serial.println(id);
   while (p != FINGERPRINT_OK) {                   // Inicia la iteracion: mientras p es distinto a huella_digital_ok, entonces hace lo siguiente:
    p = finger.getImage();                        // Se le asigna a la variable p, la obtencion de la imagen.
    switch (p) {                                  // Seleccionar p
    case FINGERPRINT_OK:                          // En caso de; huella digital correcta, hace lo siguiente:
      Serial.println("Imagen tomada");            // Imprime el  mensaje en el monitor serial.
      break;                                      // Realiza el  corte.
    case FINGERPRINT_NOFINGER:                    // En caso de; sin huella dactilar, hace lo siguiente:
      Serial.println(".");                        // Imprime el  mesaje en el monitor serial.
      break;                                      // Realiza el  corte.
    case FINGERPRINT_PACKETRECIEVEERR:            // En caso de; error al recibir el paquete de huella dactilar, hace lo siguiente:
      Serial.println("Error de comunicación");    // Imprime el  mensaje en el monitor serial.
      break;                                      // Realiza el  corte.
    case FINGERPRINT_IMAGEFAIL:                   // En caso de; fallo de imagen de la huella dactilar, hace lo siguiente:
      Serial.println("Error de imagen");          // Imprime el  mensaje en el monitor serial.
      break;                                      // Realiza el  corte.
    default:                                      // En caso de; que no haya coincidencia con ninguno de los casos anteriores, hace lo siguiente:
      Serial.println("Error desconocido");        // Imprime el  mensaje en el monitor serial.    
      break;                                      // Realiza el  corte.
    }
  }
// Si se establecio el registro del dedo, se pasa a la siguiente instruccion:
// Plantilla: Extraccion de la huella; islas, crestas, valles, bifurcacion, nucleos.
// Se covierte la primera imagen de la huella dactilar
  p = finger.image2Tz(1);          // Convierte la imagen de la huella en una plantilla y almacena el resultado en la variable.
  switch (p) {
    case FINGERPRINT_OK:                                    // En caso de; huella digital correcta, hace lo siguiente:
      Serial.println("Imagen convertida");                  // Imprime el  mensaje en el monitor serial.
      break;                                                // Realiza el  corte.
    case FINGERPRINT_IMAGEMESS:                             // En caso de; imagen de huella confusa, hace lo siguiente:
      Serial.println("Imagen demasiado desordenada");       // Imprime el  mensaje en el monitor serial.
      return p;                                             // Retorna en  p
    case FINGERPRINT_PACKETRECIEVEERR:                      // En caso de; error al recibir el paquete de huella dactilar, hace lo siguiente:
      Serial.println("Error de comunicación");              // Imprime el  mensaje en el monitor serial.
      return p;                                             // Retorna en  p
    case FINGERPRINT_FEATUREFAIL:                           // En caso de; fallo de caracteristica de huella, hace lo siguiente
      Serial.println("No se pudieron encontrar las características de la huella digital");  // Imprime el  mensaje en el monitor serial.
      return p;                                             // Retorna en  p
    case FINGERPRINT_INVALIDIMAGE:                          // En caso de; imagen de huella digital no validad, hace lo siguiente
      Serial.println("No se pudieron encontrar las características de la huella digital");  // Imprime el  mensaje en el monitor serial.
      return p;                                             // Retorna en  p
    default:                                                // En caso de; que no haya coincidencia con ninguno de los casos anteriores, hace lo siguiente:
      Serial.println("Error desconocido");                  // Imprime el  mensaje en el monitor serial.
      return p;                                             // Retorna en  p
  }  
// Si se establecio la conversion la primera imagen de la huella dactilar , se pasa a la siguiente instruccion: 
  Serial.println("Quitar el dedo");                         // Imprime el  mensaje en el monitor serial.
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Vuelva a colocar el mismo dedo");         // Imprime el  mensaje en el monitor serial.
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen tomada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicacion");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Error de imagen"); 
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
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagen demasiado desordenada"); 
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicacion");
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
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Las huellas dactilares no coinciden");
    return p;
  } else {
    Serial.println("Error desconocido");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("¡Almacenado!"); 
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicacion"); 
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("No se pudo almacenar en esa ubicación");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error al escribir en flash");
    return p;
  } else {
    Serial.println("Error desconocido");
    return p;
  }

  return true;
}
