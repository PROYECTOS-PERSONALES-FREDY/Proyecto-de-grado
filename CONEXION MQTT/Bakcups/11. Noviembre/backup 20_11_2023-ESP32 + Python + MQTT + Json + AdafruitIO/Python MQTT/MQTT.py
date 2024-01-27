#Libreria MQTT
import paho.mqtt.client as mqtt
#Libreria para conectar con la pagina web Adafruit_IO
from Adafruit_IO import Client, Feed, Data, RequestError
#Se importa libreria para guardar informacion en un archivo json
import json
#librerias para facilitar el procesado de datos
import datetime
import time
import numpy as np


#pip install numpy
#pip install paho-mqtt
#pip3 install adafruit-io

#Credenciales de la pagina web Adafruit_IO

# The above code is setting up the Adafruit IO username and key for authentication. It then creates an
# instance of the Adafruit IO client using the provided username and key.
ADAFRUIT_IO_USERNAME = 'FredyxD25'
ADAFRUIT_IO_KEY = 'aio_rTEy86Jd3JCL3A4sN0axA5rDPblM'
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

#Ip del servidor proxy

servidor = "192.168.1.25"
#servidor = "10.83.83.25"


# The above code is creating and retrieving Adafruit IO feeds. It first tries to retrieve the feeds
# named 'humedad', 'celsius', 'fahrenheit', and 'password'. If any of these feeds do not exist, it
# creates them using the `create_feed()` method. Finally, it retrieves the 'teclado' feed.
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
    
#Definicion listas y vectores para limpiar "mugre" de datos recibidos, bucles entre otros

Numero_str=""
clave=['0','0','0','0','0','0']
menu=['#','#']
menu2=['#','#','#']
inicio=['0','0','0','0','0']
dato_submenu='0'
datos_simples="+"
datos_dobles="+"
guardar_menu="+"
guardar_menu2="+"
fecha_hora=''
num_celsius=0
num_humedad=0
suma_celsius=0
suma_humedad=0
guardar_celsius = []
guardar_humedad = []
json_celsius = []
json_humedad = []
unavez=0
unavez2=0
principal=False
configuracion=False
submenu1=False
submenu2=False
submenu3=False
envio_datos1=True
envio_datos2=False
a=False

#Funcion para fecha y hora
def list_to_float_fecha_hora(aux):
    """
    The function takes a string as input, splits it into a list, and extracts a specific float value
    from the list.
    
    :param aux: The parameter "aux" is a string that contains a list of values separated by the letter
    'b'. Each value in the list is further separated by a semicolon ';'. The function is designed to
    extract a specific value from this list and convert it to a float
    :return: a float value.
    """
    #Definen Variables
    Datos_List_Float=list
    Vector_List_Float=[]
    Vector_List_Float_2=[]
    Numero_Float=0
    #Codigo
    Datos_List_Float=aux
    Vector_List_Float=np.array(Datos_List_Float.split('b'))
    Vector_List_Float_2=np.array(Vector_List_Float[1].split(';'))
    Numero_Float=Vector_List_Float_2[1]
    Numero_Float=Numero_Float[:-1]
    return Numero_Float

#Funcion para variable de los sensores recibidas
def list_to_float_dato(aux):
    """
    The function takes a string as input, splits it into a list, extracts a specific element, converts
    it to a float, and returns the float value.
    
    :param aux: The parameter "aux" is a string that contains a list of numbers separated by the letter
    'b' and each number is separated by a semicolon ';'
    :return: a float value.
    """
    #Definen Variables
    Datos_List_Float=list
    Vector_List_Float=[]
    Vector_List_Float_2=[]
    Numero_Float=0
    #Codigo
    Datos_List_Float=aux
    Vector_List_Float=np.array(Datos_List_Float.split('b'))
    Vector_List_Float_2=np.array(Vector_List_Float[1].split(';'))
    Numero_Float=Vector_List_Float_2[0]
    Numero_Float=float(Numero_Float[1:])
    return Numero_Float
 
#Funcion para eliminar caracteres no deseados y devolver un string
def list_to_string(aux):
    """
    The function "list_to_string" takes a list as input, converts it to a string, splits it at the
    letter 'b', and returns the second element of the resulting array as a string.
    
    :param aux: The parameter "aux" is a string that contains a list of elements separated by the letter
    'b'
    :return: a string representation of the second element in the input list.
    """
    #Definen Variables
    Datos_List_String=list
    Vector_List_String=[]
    Numero_String=0
    #Codigo
    Datos_List_String=aux
    Vector_List_String=np.array(Datos_List_String.split('b'))
    Numero_String=str(Vector_List_String[1])
    return Numero_String

#Funcion para recibir el numero enviado desde el teclado de la pagina web Adafruit_IO
def get_adafruitio_message():
    """
    The function `get_adafruitio_message` receives a message from Adafruit IO and returns it as a
    string.
    :return: a string representation of the data received from the Adafruit IO platform.
    """
    data = aio.receive(teclado.key)
    Numero_str=str(data[3])
    return (Numero_str)

#Funcion para limitar el vector a un tamaño en especifico
def envio_datos_adafritio(vector,x1):
    """
    The function `envio_datos_adafritio` removes elements from a vector based on a given condition.
    
    :param vector: The "vector" parameter is a list of elements
    :param x1: The parameter x1 represents the index position in the vector
    """
    client.loop_start()#------
    if (len(vector)==(x1+1)):
        vector.pop()
    elif (len(vector)>=(x1+2)):    
        vector.pop()
        vector.pop()

#Funcion para enviar y saltar el primer dato enviado por la pagina web
def antirebote(vector):
    """
    The function `antirebote` checks if the first element of the `vector` list is equal to the current
    Adafruit IO message, and if not, it inserts the current message at the beginning of the list.
    
    :param vector: The parameter "vector" is a list that contains elements
    """
    global unavez2
    client.loop_start()#------
    if(str(get_adafruitio_message())!=vector[0]):
        vector.insert(0,str(get_adafruitio_message()))
    unavez2=1

#Funcion para volver al inicio, ademas de reiniciar todas las variables y submenus 
def back():
    """
    The function "back" resets global variables to their initial values.
    """
    global unavez
    global unavez2
    global clave
    global menu
    global menu2
    global submenu1
    global submenu2
    global submenu3
    global principal
    global configuracion
    global envio_datos1
    global a
    principal = False
    configuracion = False

    submenu1 = False
    submenu2 = False
    submenu3 = False

    envio_datos1 = True
    a=False

    clave=['0','0','0','0','0','0']
    menu=['#','#']
    menu2=['#','#','#']

    unavez=0
    unavez2=0
#Funcion de funcionamiento de los submenus
def submenus(vector):
    """
    The function `submenus` handles different submenus and performs various actions based on the input
    vector.
    
    :param vector: The parameter "vector" is a list that contains three elements. It is used to store
    and manipulate input values
    """
    global guardar_menu
    global datos_simples
    global datos_dobles
    global envio_datos1
    global envio_datos2
    global submenu1
    global submenu2
    global submenu3

    #Se llaman las funciones
    antirebote(vector)
    envio_datos_adafritio(vector,3) 
    print(vector)
  
    #Codigo Registro de huella
    if submenu1 == True and submenu2 == False and submenu3 == False and vector[0] == '#' and vector[1] != '#' and vector[2] != '#' and vector[1] != '0' and vector[2] != '0' and vector[0] != '*' and guardar_menu != str(get_adafruitio_message()):
        aio.send_data(password.key,datos_dobles)
        time.sleep(0.5)
        print("Enviando...")
        aio.send_data(password.key,"\nEsperando un dedo válido para inscribir con ID #"+str(datos_dobles)+"...\n")
        client.publish("sensor/huella/contraseña",str('#'+str(vector[2])+str(vector[1])))
        envio_datos1 == False
        envio_datos2 == False
        guardar_menu=vector[0]
        back()
            
    if submenu1 == True and submenu2 == False and submenu3 == False and vector[0] == '#' and vector[1] != '#' and vector[2] == '#' and vector[1] != '0' and vector[2] != '0' and vector[0] != '*' and guardar_menu != str(get_adafruitio_message()):
        aio.send_data(password.key,datos_simples)
        time.sleep(0.5)
        print("Enviando...")
        aio.send_data(password.key,"\nEsperando un dedo válido para inscribir con ID #"+str(datos_simples)+"...\n")
        client.publish("sensor/huella/contraseña",str('#'+str(vector[2])+str(vector[1])))
        envio_datos1 == False
        envio_datos2 == False
        guardar_menu=vector[0]
        back()

    #Codigo Eliminar huella
    if submenu1 == False and submenu2 == True and submenu3 == False and vector[0] == '#' and vector[1] != '#' and vector[2] != '#' and vector[1] != '0' and vector[2] != '0' and vector[0] != '*' and guardar_menu != str(get_adafruitio_message()):
        aio.send_data(password.key,datos_dobles)
        time.sleep(0.5)
        aio.send_data(password.key,"\nEliminando huella con ID #"+str(datos_dobles)+"...\n")
        client.publish("sensor/huella/contraseña",str('#'+str(vector[2])+str(vector[1])))
        guardar_menu=vector[0]
        envio_datos1 == False
        envio_datos2 == False
        back()
            
    if submenu1 == False and submenu2 == True and submenu3 == False and vector[0] == '#' and vector[1] != '#' and vector[2] == '#' and vector[1] != '0' and vector[2] != '0' and vector[0] != '*' and guardar_menu != str(get_adafruitio_message()):
        aio.send_data(password.key,datos_simples)
        time.sleep(0.5)
        aio.send_data(password.key,"\nEliminando huella con ID #"+str(datos_simples)+"...\n")
        client.publish("sensor/huella/contraseña",str('#'+str(vector[2])+str(vector[1])))
        guardar_menu=vector[0]
        envio_datos1 == False
        envio_datos2 == False
        back()
    
    #Envio de variables
    elif vector[0] != '#' and vector[1] != '#' and vector[2] == '#' and envio_datos2 == True and guardar_menu != str(get_adafruitio_message()):
        datos_dobles = str(vector[1])+str(vector[0])
        aio.send_data(password.key,datos_dobles)
        print("dos numeros")
        guardar_menu=vector[0]
        envio_datos2=False

    elif vector[0] != '#' and vector[1] == '#' and envio_datos1 == True and guardar_menu != str(get_adafruitio_message()):
        datos_simples = str(vector[0])
        print("un numero")
        aio.send_data(password.key,datos_simples)
        guardar_menu=vector[0]
        envio_datos1=False
        envio_datos2=True

    #Codigo eliminar base de datos
    if submenu1 == False and submenu2 == False and submenu3 == True and get_adafruitio_message()=='1':
        aio.send_data(password.key,'1')
        time.sleep(0.5)
        client.publish("sensor/huella/contraseña",'?')
        back()

    if vector[0] == '*':
        print("REINICIANDO")
        client.publish("sensor/huella/contraseña","$")
        back()

    #Reiniciar envio de variables
    if vector[0] != '#' and vector[1] != '#' and vector[2] != '#':
        vector[0] = '#' 
        vector[1] = '#'
        vector[2] = '#'
        envio_datos1=True
    if vector[2]=='0':
        vector[0] = '#' 
        vector[1] = '#'
        vector[2] = '#'
        envio_datos1=True
    if vector[0]=='#' and vector[1] == '0' and vector[2] == '#':
        aio.send_data(password.key,"\nNo se puede enviar ese numero, intente con otro por favor...\n")
        vector[0] = '#' 
        vector[1] = '#'
        vector[2] = '#'
        envio_datos1=True
    
#Funcion principal para el envio de datos
def password_send():
    """
    The function `password_send` contains multiple while loops that handle different menus and submenus
    based on user input.
    """
    global principal
    global configuracion
    global submenu1
    global submenu2
    global submenu3
    global guardar_menu
    global guardar_menu2

    #Se oprime * para introducir la contraseña de administrador
    while principal == False and configuracion==False and submenu1 == False and submenu2 == False and submenu3 == False:
        print('menu principal')
        if(str(get_adafruitio_message())=='*'):
            aio.send_data(password.key,"Ingrese contraseña:")
            principal = True
            
    
    while principal == True and configuracion==False and submenu1 == False and submenu2 == False and submenu3 == False:
        #Si la clave es correcta se ingresa al siguiente menu
        if clave == ['#', '8', '3', '2', '7', '2']:
        #if clave[0] == '0':
            aio.send_data(password.key,"\nClave correcta\n")
            client.publish("sensor/huella/contraseña","*") #Se envia el caracter para informarle a la ESP32 el acceso al menu de administrador
            clave.insert(0,"-")
            principal = False

        #Una vez pulsado # y realizado el envio, se reinicia el vector
        elif clave[0] == '#' and clave[1] != '-':
            aio.send_data(password.key,'\nClave incorrecta\n')
            clave.insert(0,"-")
        
        #Se llaman las funciones
        antirebote(clave)
        envio_datos_adafritio(clave,6)
        print(clave)

    #Se inicia el primer bucle para el menu de administrador
    while principal == False and configuracion==True and submenu1 == False and submenu2 == False and submenu3 == False:
        print("Menu administrador")
        #Se llaman las funciones
        antirebote(menu)
        envio_datos_adafritio(menu,2)
        print(menu)
        #Primer submenu
        if menu == ['#', '1']:
            client.publish("sensor/huella/contraseña","1") #Se envia el caracter para informarle a la ESP32 el acceso al menu de administrador
            time.sleep(0.5)
            configuracion = False
            a=False

        elif menu == ['#', '2']:
            client.publish("sensor/huella/contraseña","2")
            time.sleep(0.5)
            configuracion = False
            a=False
                
        elif menu == ['#', '3']:
            client.publish("sensor/huella/contraseña","3")
            time.sleep(0.5)
            configuracion = False
            a=False

        elif menu[0] != '#' and guardar_menu != str(get_adafruitio_message()) and guardar_menu2 != menu[1] and (menu[0] == '1' or menu[0] == '2' or menu[0] == '3'):
            datos_simples = str(menu[0])
            aio.send_data(password.key,datos_simples)
            guardar_menu=menu[0]
            guardar_menu2=menu[0]

        if menu[0] == '*':
            print("REINICIANDO")
            client.publish("sensor/huella/contraseña","$")
            back()

        if menu[0] != '#' and menu[1] != '#': 
            menu[0] = '#'
            menu[1] = '#'

    while principal == False and configuracion==False and submenu1 == True and submenu2 == False and submenu3 == False:
        submenus(menu2)
    while principal == False and configuracion==False and submenu1 == False and submenu2 == True and submenu3 == False:
        submenus(menu2)
    while principal == False and configuracion==False and submenu1 == False and submenu2 == False and submenu3 == True:
        submenus(menu2)

def on_message(client, userdata, msg):
    """
    The `on_message` function is a Python function that handles incoming messages from a client. It
    performs various operations based on the topic of the message, such as storing temperature and
    humidity data, sending data to Adafruit IO, and handling menu options for a fingerprint sensor.
    
    :param client: The `client` parameter is the MQTT client object that is used to connect to the MQTT
    broker and publish/subscribe to topics
    :param userdata: The `userdata` parameter is a user-defined data that can be passed to the
    `on_message` function. It can be used to store any additional information or context that you want
    to access within the function
    :param msg: The `msg` parameter is the message received from the MQTT broker. It contains
    information such as the topic of the message and the payload (data) of the message. The code checks
    the topic of the message and performs different actions based on the topic
    """
    
    global num_celsius
    global num_humedad
    global suma_celsius
    global suma_humedad
    global guardar_celsius
    global unavez
    global fecha_hora
    global configuracion
    global submenu1
    global submenu2
    global submenu3
    global a
    numero_datos_celsius=300
    numero_datos_humedad=300
    
    if msg.topic == "sensor/DHT11/temperature_celsius":
        DatosCelsius = list_to_float_dato(str(msg.payload))
        fecha_hora = list_to_float_fecha_hora(str(msg.payload))
        print(f"{len(guardar_celsius)}")
        print(f"\nCelsius: {DatosCelsius}")
        if (num_celsius <= numero_datos_celsius):
            guardar_celsius.append(DatosCelsius)
            suma_celsius = suma_celsius + DatosCelsius
            print(f"vector + posicion: {guardar_celsius[num_celsius]} {num_celsius}")
            print(f"suma total: {suma_celsius}")
            if(num_celsius>0):
                prom_celsius = (suma_celsius/(num_celsius+1))
                print(f"promedio total: {prom_celsius}\n")
                
            num_celsius = num_celsius + 1
        if (num_celsius > numero_datos_celsius):
            json_celsius.insert(0,"Dato: "+ str(prom_celsius)+";"+"Fecha y Hora:"+fecha_hora)
            with open("Celsius.json",'w') as outfile:
                json.dump(json_celsius,outfile,indent=4)
            aio.send_data(celsius.key, str(prom_celsius))
            num_celsius = 0
            prom_celsius = 0
            suma_celsius = 0   
        envio_datos_adafritio(guardar_celsius,numero_datos_celsius)
        print(f"{len(guardar_celsius)}\n")

    if msg.topic == "sensor/DHT11/humidity":
        DatosHumedad = list_to_float_dato(str(msg.payload))
        fecha_hora = list_to_float_fecha_hora(str(msg.payload))
        print(f"\nHumedad: {DatosHumedad}")
        if (num_humedad <= numero_datos_humedad):
            guardar_humedad.append(DatosHumedad)
            suma_humedad = suma_humedad + DatosHumedad
            print(f"vector + posicion: {guardar_humedad[num_humedad]} {num_humedad}")
            print(f"suma total: {suma_humedad}")
            if(num_humedad>0):
                prom_humedad = (suma_humedad/(num_humedad+1))
                print(f"promedio total: {prom_humedad}\n")
            num_humedad = num_humedad + 1
        if (num_humedad > numero_datos_humedad):
            json_humedad.insert(0,"Dato: "+ str(prom_humedad)+";"+"Fecha y Hora:"+fecha_hora)
            with open("Humedad.json",'w') as outfile:
                json.dump(json_humedad,outfile,indent=4)

            aio.send_data(humedad.key, str(DatosHumedad))
            num_humedad = 0
            prom_humedad = 0
            suma_humedad = 0 
        envio_datos_adafritio(guardar_humedad,numero_datos_humedad)
        print(f"{len(guardar_humedad)}\n")
        
    if msg.topic == "sensor/huella/submenu":
        dato_submenu=list_to_string(str(msg.payload)).strip("''")
        print(f"control: {dato_submenu}")
        
        if (unavez==0):
            print("UNAVEEEZZzzz")
            dato_submenu='0'
            print(dato_submenu)
        unavez=1
        if(str(dato_submenu)=='!!'):
            back()
        if(str(dato_submenu)=='!0'):
            print(f"inicio: {dato_submenu}")
            print("configuracion 1: ", configuracion)
            configuracion = True
            if a==False:
                aio.send_data(password.key,'Ajustes de sensor de huella:\n 1. Registrar Huella\n 2. Eliminar Huella\n 3. Eliminar base de datos\nSeleccione la opcion y envie con #, utilice * para volver al inicio\n\n ')
                a=True
        if(str(dato_submenu)=='!1'):
            print(f"inicio: {dato_submenu}")
            submenu1 = True
            a=False
            aio.send_data(password.key,'Submenu 1:\nEscriba el número de identificación (del 1 al 99), utilice * para volver al inicio\n\n ')
        if(str(dato_submenu)=='!2'):
            print(f"inicio: {dato_submenu}")
            submenu2 = True
            a=False
            aio.send_data(password.key,'Submenu 2:\nEscriba el número de identificación (del 1 al 99) que desea eliminar y envielo con #\n, utilice * para volver al inicio\n\n ')
        if(str(dato_submenu)=='!3'):
            print(f"inicio: {dato_submenu}")
            submenu3 = True
            a=False
            aio.send_data(password.key,'Submenu 3:\nEliminando todas las huellas\nPresiona 1 para continuar, utilice * para volver al inicio\n\n ')                                
        
    
    if msg.topic == "sensor/huella/inicio":
        
        envio_datos_adafritio(inicio,4)
        inicio.insert(0,list_to_string(str(msg.payload)).strip("''"))

        if(str(inicio[0])!=str(inicio[1])):

#--------------------Buscar huella-------------------------------
            if(str(inicio[0])=='No se ha detectado ninguna huella'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[3])=='Encontrada coincidencia con ID# '):
                print(f"inicio: {inicio}")
                envio = inicio[3]+inicio[2]
                aio.send_data(password.key,envio)     
                time.sleep(1)
            if(str(inicio[0])=='No se encontro ninguna coincidencia'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='Sensor sin huellas registradas'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
#--------------------Registrar Huella----------------            
            if(str(inicio[0])=='Imagen tomada'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='Quitar el dedo'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='Vuelva a colocar el mismo dedo'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='Huella registrada'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='No se pudo almacenar en esa ubicación'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
#-----------------------Eliminar Huella---------------------------------
            if(str(inicio[0])=='Huella eliminada correctamente'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0]+"\nVolviendo al inicio...\n")
                time.sleep(1)
#-----------------------Eliminar Base de datos---------------------------------
            if(str(inicio[0])=='Base de datos eliminada...'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0]+'\n saliendo del menu administrador...')
                time.sleep(1)
#-----------------------Errores------------------------------------------
            if(str(inicio[0])=='No se encontro el sensor de huellas'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='Error, falla en la imagen'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='Imagen demasiado desordenada'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='No se encontraron las caracteristicas de la huella'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='Error de imagen'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='Error de comunicacion'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0]) 
                time.sleep(1)
            if(str(inicio[0])=='Error al escribir en la memoria interna'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='Error ID'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)
            if(str(inicio[0])=='Error rescribiendo la memoria interna'):
                print(f"inicio: {inicio}")
                aio.send_data(password.key,inicio[0])
                time.sleep(1)

def on_connect(client, userdata, flags, rc):
    """
    The function `on_connect` is used to handle the connection event in MQTT and print a message
    indicating the connection status.
    
    :param client: The client parameter is the MQTT client object that is used to connect to the MQTT
    broker and perform various operations such as publishing and subscribing to topics
    :param userdata: The userdata parameter is an optional parameter that allows you to pass any custom
    data to the on_connect function. It can be used to store any additional information or objects that
    you may need within the function
    :param flags: The "flags" parameter in the on_connect function is a dictionary that contains flags
    indicating the status of the connection. These flags can be used to determine if the connection was
    successful or if there were any issues
    :param rc: The parameter "rc" stands for "return code" and it indicates the result of the connection
    attempt. The return code can have the following values:
    """
    print("Se conecto con mqtt " + str(rc))
    client.subscribe("sensor/#")

# The above code is setting up a MQTT client in Python. It creates an instance of the `mqtt.Client()`
# class and sets the `on_connect` and `on_message` callback functions. It then connects to a MQTT
# broker at the specified server and port, and starts a loop to handle incoming messages.
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(servidor, 1883)
client.loop_start()

# The above code is reading data from two JSON files, "Humedad.json" and "Celsius.json", and storing
# the contents of each file in separate variables, json_humedad and json_celsius, respectively.
with open("Humedad.json") as file:
    json_humedad = json.load(file)

# The above code is opening a file named "Celsius.json" and loading its contents into a variable
# called `json_celsius` using the `json.load()` function.
with open("Celsius.json") as file:
    json_celsius = json.load(file)

# The above code is creating an infinite loop that continuously calls the function `password_send()`.
while True:
    password_send()
