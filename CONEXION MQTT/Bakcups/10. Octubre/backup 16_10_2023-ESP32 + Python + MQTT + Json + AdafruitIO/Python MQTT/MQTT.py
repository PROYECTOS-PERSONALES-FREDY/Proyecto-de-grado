#https://www.youtube.com/watch?v=5rHWeV0dwxo&t=1s&ab_channel=FusionAutomate

import paho.mqtt.client as mqtt
from Adafruit_IO import Client, Feed, Data, RequestError
import datetime
import time
import numpy as np
#pip install numpy

ADAFRUIT_IO_USERNAME = 'FredyxD25'
ADAFRUIT_IO_KEY = 'aio_rTEy86Jd3JCL3A4sN0axA5rDPblM'
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

#Envio de datos a adafruitio
try:
    humedad = aio.feeds('humedad')
except RequestError:
    feed = Feed(name="humedad")
    humedad = aio.create_feed(feed)
try:
    celsius = aio.feeds('celsius')
except RequestError:
    feed = Feed(name="celsius") 
    celsius = aio.create_feed(feed)   
try:
    fahrenheit = aio.feeds('fahrenheit')  
except RequestError:
    feed = Feed(name="fahrenheit")
    fahrenheit = aio.create_feed(feed)
try:
    password = aio.feeds('password')
except RequestError:
    feed = Feed(name="password")
    password = aio.create_feed(feed)        
#Recepcion de datos    
try:
    teclado = aio.feeds('teclado')
except RequestError:
    feed = Feed(name="teclado")
    teclado = aio.create_feed(feed)       
    
#Definicion listas y vectores para limpiar "mugre" de datos recibidos
Numero_str=""
clave=[0,0,0,0,0,0]
menu=[0,0]
configuracion=False

def list_to_float(aux):
    #Definen Variables
    Datos_List_Float=list
    Vector_List_Float=[]
    Numero_Float=0
    #Codigo
    Datos_List_Float=aux
    Vector_List_Float=np.array(Datos_List_Float.split('b'))
    Numero_Float=eval(Vector_List_Float[1])
    Numero_Float=float(Numero_Float)
    return Numero_Float
 
def list_to_string(aux):
    #Definen Variables
    Datos_List_String=list
    Vector_List_String=[]
    Numero_String=0
    #Codigo
    Datos_List_String=aux
    Vector_List_String=np.array(Datos_List_String.split('b'))
    #Numero_String=eval(Vector_List_String[1])
    Numero_String=str(Vector_List_String[1])
    return Numero_String

def get_adafruitio_message():
    data = aio.receive(teclado.key)
    Numero_str=str(data[3])
    return (Numero_str)

def password_send():
    if (len(clave)>5):
        clave.pop()
    clave.insert(0,str(get_adafruitio_message()))
    data = str(clave[5])+str(clave[4])+str(clave[3])+str(clave[2])+str(clave[1])+str(clave[0])
    print(clave)
    print(data)
    if(str(clave[0])!=str(clave[1])):
        print("enviando datos")
        aio.send_data(password.key,data)

        #if clave == ['#', '8', '3', '2', '7', '2']:
        if clave[0] == '1':
            client.publish("sensor/huella/contraseña","*")
            print("Envio clave...")
            aio.send_data(password.key,'Ajustes de sensor de huella:\n 1. Registrar Huella\n 2. Eliminar Huella\n 3. Eliminar base de datos\nSeleccione la opcion y envie con #')
            configuracion = True
            menu.insert(0,str(get_adafruitio_message()))
            while configuracion==True:
                if (len(menu)==2):
                    menu.pop()
                elif (len(menu)==3):    
                    menu.pop()
                    menu.pop()
                menu.insert(0,str(get_adafruitio_message()))    
                print(menu)
                data2 = str(menu[1])+str(menu[0])
                if(str(menu[0])!=str(menu[1])):
                    print("Entrando en submenu...")
                    aio.send_data(password.key,data2)
                    if menu == ['#', '1']:
                        aio.send_data(password.key,'Submenu 1:')
                        client.publish("sensor/huella/contraseña","1")
                    if menu == ['#', '2']:
                        aio.send_data(password.key,'Submenu 2:')
                        client.publish("sensor/huella/contraseña","2")
                    if menu == ['#', '3']:
                        aio.send_data(password.key,'Submenu 3:')
                        client.publish("sensor/huella/contraseña","3")
                else:
                    print("no se envian datos")   
    else:
        print("no se envian datos")

def on_connect(client, userdata, flags, rc):
    print("Se conecto con mqtt " + str(rc))
    client.subscribe("sensor/#")

def on_message(client, userdata, msg):
    '''
    if msg.topic == "sensor/DHT11/temperature_celsius":
        DatosCelsius = list_to_float(str(msg.payload))
        print(f"Celsius: {DatosCelsius}")
        aio.send_data(celsius.key, str(DatosCelsius))

    if msg.topic == "sensor/DHT11/temperature_fahrenheit":
        DatosFahrenheit = list_to_float(str(msg.payload))
        print(f"Fahrenheit: {DatosFahrenheit} \n")
        aio.send_data(fahrenheit.key, str(DatosFahrenheit))
        
    if msg.topic == "sensor/DHT11/humidity":
        DatosHumedad = list_to_float(str(msg.payload))
        print(f"Humedad: {DatosHumedad}")
        aio.send_data(humedad.key, str(DatosHumedad))
    '''
    if msg.topic == "sensor/huella/submenu":
        submenu = list_to_string(str(msg.payload))
        print(f"submenu: {submenu}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.221.100", 1883)
client.loop_start()

while True:
    password_send()
    #client.publish("sensor/huella/contraseña","*")