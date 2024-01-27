#https://www.youtube.com/watch?v=5rHWeV0dwxo&t=1s&ab_channel=FusionAutomate

import paho.mqtt.client as mqtt
from Adafruit_IO import Client, Feed, Data, RequestError
import datetime
import time
import numpy as np

#pip install numpy
#pip install paho-mqtt
#pip3 install adafruit-io

ADAFRUIT_IO_USERNAME = 'FredyxD25'
ADAFRUIT_IO_KEY = 'aio_rTEy86Jd3JCL3A4sN0axA5rDPblM'
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

servidor = "192.168.0.25"

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
clave=['0','0','0','0','0','0']
menu=['0','0']
menu2=['0','0']
inicio=['0','0','0','0','0']
dato_submenu='0'

global configuracion
global submenu
global unavez
unavez=0
configuracion=False
submenu=False


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
    
def envio_datos_adafritio(vector,x1):
    client.loop_start()#------
    if (len(vector)==(x1+1)):
        vector.pop()
    elif (len(vector)>=(x1+2)):    
        vector.pop()
        vector.pop()

def antirebote(vector):
    client.loop_start()#------
    if(str(get_adafruitio_message())!=vector[0]):
        vector.insert(0,str(get_adafruitio_message()))

def antirebote_dosnumeros(vector):
    client.loop_start()#------
    if(str(get_adafruitio_message())!=vector[0]) and str(get_adafruitio_message())!='*':
        vector.insert(0,str(get_adafruitio_message()))
    if(str(get_adafruitio_message())=='*'):
        vector.insert(1,vector[0])

def bool_configuration_True():
    global configuracion
    configuracion = True
def bool_submenu_True():
    global submenu
    submenu = True
def bool_configuration_False():
    global configuracion
    '''global unavez
    unavez=0'''
    configuracion = False
def bool_submenu_False():
    global submenu
    submenu = False

def password_send():
    global configuracion
    global submenu

    antirebote(clave)
    envio_datos_adafritio(clave,6)
    #constraseña_censura(clave)
    print(clave)
        
    if clave == ['#', '8', '3', '2', '7', '2']:
    ##if clave[0] == '0':
        data = str(clave[5])+str(clave[4])+str(clave[3])+str(clave[2])+str(clave[1])+str(clave[0])
        aio.send_data(password.key,"******")
        client.publish("sensor/huella/contraseña","*")
        clave.insert(0,"-")
    
    elif clave[0] == '#' and clave[1] != '-':
        aio.send_data(password.key,'\nClave incorrecta\n')
        clave.insert(0,"-")
        
    while configuracion==True:

        antirebote(menu)
        envio_datos_adafritio(menu,2)  

        print(menu)

        if menu == ['#', '1']:
            datos = str(menu[1])+str(menu[0])
            client.publish("sensor/huella/contraseña","1")
            time.sleep(0.5)
            submenu = True 

            while submenu == True:

                antirebote_dosnumeros(menu2)
                envio_datos_adafritio(menu2,3) 
                print(menu2)
                
                if menu2[0] == '#':
                    if menu2[1] != '0' and menu2[1] != '#':
                        if menu2[2] != '0' and menu2[2] != '#':
                            datos = str(menu2[2])+str(menu2[1])+str(menu2[0])
                            aio.send_data(password.key,datos)
                            time.sleep(0.5)
                            aio.send_data(password.key,"\nEsperando un dedo válido para inscribir con ID #"+str(menu2[2]+menu2[1]+"...\n"))
                            client.publish("sensor/huella/contraseña",str('#'+menu2[2]+menu2[1]))
                            bool_configuration_False()
                            bool_submenu_False()
                            clave.insert(0,"-")
                            menu.insert(0,"0")
                            menu2.insert(0,"0")

        if menu == ['#', '2']:
            datos = str(menu[1])+str(menu[0])
            client.publish("sensor/huella/contraseña","2")
            time.sleep(0.5)
            submenu = True

            while submenu == True:

                antirebote_dosnumeros(menu2)
                envio_datos_adafritio(menu2,3) 
                print(menu2)
                if menu2[0] == '#':
                    if menu2[1] != '0' and menu2[1] != '#':
                        if menu2[2] != '0' and menu2[2] != '#':
                            datos = str(menu2[2])+str(menu2[1])+str(menu2[0])
                            aio.send_data(password.key,datos)
                            time.sleep(0.5)
                            client.publish("sensor/huella/contraseña",str('#'+menu2[2]+menu2[1]))
                            bool_configuration_False()
                            bool_submenu_False()
                            clave.insert(0,"-")
                            menu.insert(0,"0")
                            menu2.insert(0,"0")

        if menu == ['#', '3']:
            asterisco='0'
            client.publish("sensor/huella/contraseña","3")
            time.sleep(0.5)
            submenu = True

            while submenu == True:

                if(get_adafruitio_message()=='*'):
                    asterisco='*'
                    aio.send_data(password.key,asterisco)
                    time.sleep(0.5)
                    client.publish("sensor/huella/contraseña",'?')
                    bool_configuration_False()
                    bool_submenu_False()
                    clave.insert(0,"-")
                    menu.insert(0,"0")
                    menu2.insert(0,"0")   


def on_message(client, userdata, msg):
    global unavez
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
        dato_submenu=list_to_string(str(msg.payload)).strip("''")
        print(f"control: {dato_submenu}")

        if (unavez==0):
            print("UNAVEEEZZzzz")
            dato_submenu='0'
            print(dato_submenu)
        unavez=1

        if(str(dato_submenu)=='!0'):
            print(f"inicio: {dato_submenu}")
            print("configuracion 1: ", configuracion)
            bool_configuration_True()
            aio.send_data(password.key,'\nAjustes de sensor de huella:\n 1. Registrar Huella\n 2. Eliminar Huella\n 3. Eliminar base de datos\nSeleccione la opcion y envie con #\n\n ')
        if(str(dato_submenu)=='!1'):
            print(f"inicio: {dato_submenu}")
            bool_submenu_True()
            aio.send_data(password.key,'\nSubmenu 1:\nEscriba el número de identificación (del 1 al 99)\nen el que desea guardar el dedo y envielo con #,\nsi es un numero doble como por ejemplo 55, presione el numero una vez y *\n\n ')
        if(str(dato_submenu)=='!2'):
            print(f"inicio: {dato_submenu}")
            bool_submenu_True()
            aio.send_data(password.key,'\nSubmenu 2:\nEscriba el número de identificación (del 1 al 99)\nque desea eliminar y envielo con #\n\n ')
        if(str(dato_submenu)=='!3'):
            print(f"inicio: {dato_submenu}")
            bool_submenu_True()
            aio.send_data(password.key,'\nSubmenu 3:\nEliminando todas las huellas\nPresiona * para continuar\n\n ')                                
        
    
    if msg.topic == "sensor/huella/inicio":
        envio_datos_adafritio(inicio,4)
        inicio.insert(0,list_to_string(str(msg.payload)).strip("''"))
        #print(f"control: {inicio}")

        if(str(inicio[0])!=str(inicio[1])):
            if(str(inicio[0])=='No se encontro el sensor de huellas'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0]) 
#--------------------------------------
            '''if(str(inicio[0])=='Imagen tomada'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,'Imagen tomada... Imagen convertida')'''

            if(str(inicio[0])=='No se ha detectado ninguna huella'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])

            if(str(inicio[0])=='Error, falla en la imagen'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])

#-----------------------------------------------------------------------
                aio.send_data(password.key,inicio[0])
            if(str(inicio[0])=='Imagen demasiado desordenada'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
            if(str(inicio[0])=='No se encontraron las caracteristicas de la huella'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])

#---------------------------------------------------------------------
            if(str(inicio[3])=='Encontrada coincidencia con ID# '):
                print(f"inicio: {inicio}")
                envio = inicio[3]+inicio[2]+inicio[1]+inicio[0]
                aio.send_data(password.key,envio)
                time.sleep(2)     

            if(str(inicio[0])=='No se encontro ninguna coincidencia'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
            
#----------------------------------------------------------------------
            '''if(str(inicio[0])=='Sensor sin huellas registradas'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])'''

#--------------------Registrar Huella----------------            
            if(str(inicio[0])=='Imagen tomada'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
            if(str(inicio[0])=='Quitar el dedo'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
            if(str(inicio[0])=='Vuelva a colocar el mismo dedo'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
            if(str(inicio[0])=='Huella registrada'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
            if(str(inicio[0])=='No se pudo almacenar en esa ubicación'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
#-----------------------------------------------------------------------
            '''if(str(inicio[0])=='Error desconocido'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])'''
            
            if(str(inicio[0])=='Error de imagen'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
            if(str(inicio[0])=='Error de comunicacion'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0]) 
            if(str(inicio[0])=='Error al escribir en la memoria interna'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])

def on_connect(client, userdata, flags, rc):
    print("Se conecto con mqtt " + str(rc))
    client.subscribe("sensor/#")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(servidor, 1883)
client.loop_start()

while True:
    password_send()
    #client.publish("sensor/huella/contraseña","#")