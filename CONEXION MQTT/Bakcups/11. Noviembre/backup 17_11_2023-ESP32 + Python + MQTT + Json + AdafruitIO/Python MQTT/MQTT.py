#https://www.youtube.com/watch?v=5rHWeV0dwxo&t=1s&ab_channel=FusionAutomate

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

#pydoc

#pip install numpy
#pip install paho-mqtt
#pip3 install adafruit-io

#Credenciales de la pagina web Adafruit_IO

ADAFRUIT_IO_USERNAME = 'FredyxD25'
ADAFRUIT_IO_KEY = 'aio_rTEy86Jd3JCL3A4sN0axA5rDPblM'
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

#Ip del servidor proxy

#servidor = "192.168.0.25"
servidor = "10.83.83.25"

#Envio de datos a adafruitio
try:
    humedad = aio.feeds('humedad') #Busca el feed humedad, si no existe lo crea, es igual para todos los feeds
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

#Funcion para fecha y hora
def list_to_float_fecha_hora(aux):
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
    data = aio.receive(teclado.key)
    Numero_str=str(data[3])
    return (Numero_str)

#Funcion para limitar el vector a un tamaño en especifico
def envio_datos_adafritio(vector,x1):
    client.loop_start()#------
    if (len(vector)==(x1+1)):
        vector.pop()
    elif (len(vector)>=(x1+2)):    
        vector.pop()
        vector.pop()

#Funcion para enviar y saltar el primer dato enviado por la pagina web
def antirebote(vector):
    global unavez2
    client.loop_start()#------
    if(str(get_adafruitio_message())!=vector[0]):
        vector.insert(0,str(get_adafruitio_message()))
        if configuracion == False and unavez2 != 0:
            aio.send_data(password.key,"******")
    unavez2=1
#Funcion para cambiar la variable a true de forma global        
def bool_principal_True():
    global principal
    principal = True  
#Funcion para cambiar la variable a true de forma global        
def bool_configuration_True():
    global configuracion
    configuracion = True  
#Funcion para cambiar la variable a true de forma global 
def bool_submenu_True():
    global submenu
    submenu = True
#Funcion para cambiar la variable a false de forma global 
def bool_principal_False():
    global principal
    principal = False
#Funcion para cambiar la variable a false de forma global 
def bool_configuration_False():
    global configuracion
    configuracion = False
#Funcion para cambiar la variable a false de forma global 
def bool_submenu_False():
    global submenu
    submenu = False

#Funcion para volver al inicio, ademas de reiniciar todas las variables y submenus 
def back():
    global unavez
    global unavez2
    global submenu1
    global submenu2
    global submenu3
    bool_principal_False()
    bool_configuration_False()
    bool_submenu_False()
    clave.insert(0,"-")
    menu.insert(0,"#")
    menu2.insert(0,"#")
    unavez=0
    unavez2=0
    #aio.send_data(password.key,'\nMenu principal\n')

def submenus(vector):
    antirebote(vector)
    envio_datos_adafritio(vector,3) 
    print(vector)
    
    if vector[0] != '#' and vector[1] != '#' and vector[2] == '#' and guardar_menu2 != str(get_adafruitio_message()):
        datos_dobles = str(vector[1])+str(vector[0])
        aio.send_data(password.key,datos_dobles)
        print("dos numeros")
        guardar_menu2=vector[0]

    elif vector[0] != '#' and vector[1] == '#' and guardar_menu2 != str(get_adafruitio_message()):
        datos_simples = str(vector[0])
        aio.send_data(password.key,datos_simples)
        guardar_menu2=vector[0]
        
    elif vector[0] != '#' and vector[1] != '#' and vector[2] != '#':
        vector[0] = '#' 
        vector[1] = '#'
        vector[2] = '#'
    
    elif vector[0] == '#' and vector[1] == '0' and vector[2] == '0':
        aio.send_data(password.key,"\nNo se puede enviar ese numero, intente con otro por favor...\n")
        vector[0] = '#' 
        vector[1] = '#'
        vector[2] = '#'

    elif vector[0] == '#' and vector[1] != '0' and vector[1] != '#' and vector[2] != '0' and vector[2] != '#':
            aio.send_data(password.key,datos_dobles)
            time.sleep(0.5)
            aio.send_data(password.key,"\nEsperando un dedo válido para inscribir con ID #"+str(datos_dobles)+"...\n")
            client.publish("sensor/huella/contraseña",str('#'+str(vector[2])+str(vector[1])))
            back()
            
    elif vector[0] == '#' and vector[1] != '0' and vector[1] != '#':
            aio.send_data(password.key,datos_simples)
            time.sleep(0.5)
            aio.send_data(password.key,"\nEsperando un dedo válido para inscribir con ID #"+str(datos_simples)+"...\n")
            client.publish("sensor/huella/contraseña",str('#'+str(vector[2])+str(vector[1])))
            back()

    elif vector[0] == '*':
        print("REINICIANDO")
        client.publish("sensor/huella/contraseña","$")
        back()

#Funcion principal para el envio de datos
def password_send():
    global principal
    global configuracion
    global submenu
    
    datos_simples="+"
    datos_dobles="+"
    guardar_menu="+"
    guardar_menu2="+"
    
    bool_configuration_True()
    bool_submenu_True()
    
    #Se oprime * para introducir la contraseña de administrador
    while principal == False and configuracion == False:
        
        if(str(get_adafruitio_message())=='*'):
            aio.send_data(password.key,"Ingrese contraseña:")
            principal = True
    
    #Se llaman las funciones
    antirebote(clave)
    envio_datos_adafritio(clave,6)
    print(clave)
    
    #Si la clave es correcta se ingresa al siguiente menu
    #if clave == ['#', '8', '3', '2', '7', '2']:
    if clave[0] == '0':
        #data = str(clave[5])+str(clave[4])+str(clave[3])+str(clave[2])+str(clave[1])+str(clave[0])
        aio.send_data(password.key,"\nClave correcta\n")
        client.publish("sensor/huella/contraseña","*") #Se envia el caracter para informarle a la ESP32 el acceso al menu de administrador
        clave.insert(0,"-")

    #Una vez pulsado # y realizado el envio, se reinicia el vector
    elif clave[0] == '#' and clave[1] != '-':
        aio.send_data(password.key,'\nClave incorrecta\n')
        clave.insert(0,"-")
    
    #Se inicia el primer bucle para el menu de administrador
    while configuracion==True:

        #Se llaman las funciones
        bool_principal_False()
        antirebote(menu)
        if menu[1] == '1' or menu[2] == '2' or menu[1] == '3':
            envio_datos_adafritio(menu,2)  
            print(menu)

        #Primer submenu
        if menu == ['#', '1']:
            client.publish("sensor/huella/contraseña","1") #Se envia el caracter para informarle a la ESP32 el acceso al menu de administrador
            time.sleep(0.5)
            submenu = True
            #Se inicia el segundo bucle para el submenu
            while submenu == True:
                submenus(menu2)
                #Se llaman las funciones
                '''antirebote(menu2)
                envio_datos_adafritio(menu2,3) 
                print(menu2)

                
                if menu2[0] != '#' and menu2[1] != '#' and menu2[2] == '#' and guardar_menu2 != str(get_adafruitio_message()):
                    datos_dobles = str(menu2[1])+str(menu2[0])
                    aio.send_data(password.key,datos_dobles)
                    print("dos numeros")
                    guardar_menu2=menu2[0]

                elif menu2[0] != '#' and menu2[1] == '#' and guardar_menu2 != str(get_adafruitio_message()):
                    datos_simples = str(menu2[0])
                    aio.send_data(password.key,datos_simples)
                    guardar_menu2=menu2[0]
                    
                elif menu2[0] != '#' and menu2[1] != '#' and menu2[2] != '#':
                    menu2[0] = '#' 
                    menu2[1] = '#'
                    menu2[2] = '#'
                
                elif menu2[0] == '#' and menu2[1] == '0' and menu2[2] == '0':
                    aio.send_data(password.key,"\nNo se puede enviar ese numero, intente con otro por favor...\n")
                    menu2[0] = '#' 
                    menu2[1] = '#'
                    menu2[2] = '#'

                elif menu2[0] == '#' and menu2[1] != '0' and menu2[1] != '#' and menu2[2] != '0' and menu2[2] != '#':
                        aio.send_data(password.key,datos_dobles)
                        time.sleep(0.5)
                        aio.send_data(password.key,"\nEsperando un dedo válido para inscribir con ID #"+str(datos_dobles)+"...\n")
                        client.publish("sensor/huella/contraseña",str('#'+str(menu2[2])+str(menu2[1])))
                        back()
                        
                elif menu2[0] == '#' and menu2[1] != '0' and menu2[1] != '#':
                        aio.send_data(password.key,datos_simples)
                        time.sleep(0.5)
                        aio.send_data(password.key,"\nEsperando un dedo válido para inscribir con ID #"+str(datos_simples)+"...\n")
                        client.publish("sensor/huella/contraseña",str('#'+str(menu2[2])+str(menu2[1])))
                        back()

                elif menu2[0] == '*':
                    print("REINICIANDO")
                    client.publish("sensor/huella/contraseña","$")
                    back()'''
                    

        if menu == ['#', '2']:
            
            client.publish("sensor/huella/contraseña","2")
            time.sleep(0.5)
            submenu = True
            while submenu == True:
                #Se llaman las funciones
                antirebote(menu2)
                envio_datos_adafritio(menu2,3) 
                print(menu2)

                
                if menu2[0] != '#' and menu2[1] != '#' and menu2[2] == '#' and guardar_menu2 != str(get_adafruitio_message()):
                    datos_dobles = str(menu2[1])+str(menu2[0])
                    aio.send_data(password.key,datos_dobles)
                    print("dos numeros")
                    guardar_menu2=menu2[0]

                elif menu2[0] != '#' and menu2[1] == '#' and guardar_menu2 != str(get_adafruitio_message()):
                    datos_simples = str(menu2[0])
                    aio.send_data(password.key,datos_simples)
                    guardar_menu2=menu2[0]
                    
                elif menu2[0] != '#' and menu2[1] != '#' and menu2[2] != '#':
                    menu2[0] = '#' 
                    menu2[1] = '#'
                    menu2[2] = '#'
                
                elif menu2[0] == '#' and menu2[1] == '0' and menu2[2] == '0':
                    aio.send_data(password.key,"\nNo se puede enviar ese numero, intente con otro por favor...\n")
                    menu2[0] = '#' 
                    menu2[1] = '#'
                    menu2[2] = '#'

                elif menu2[0] == '#' and menu2[1] != '0' and menu2[1] != '#' and menu2[2] != '0' and menu2[2] != '#':
                        aio.send_data(password.key,datos_dobles)
                        time.sleep(0.5)
                        aio.send_data(password.key,"\nEliminando huella con ID #"+str(datos_dobles)+"...\n")
                        client.publish("sensor/huella/contraseña",str('#'+str(menu2[2])+str(menu2[1])))
                        back()
                        
                elif menu2[0] == '#' and menu2[1] != '0' and menu2[1] != '#':
                        aio.send_data(password.key,datos_simples)
                        time.sleep(0.5)
                        aio.send_data(password.key,"\nEliminando huella con ID #"+str(datos_simples)+"...\n")
                        client.publish("sensor/huella/contraseña",str('#'+str(menu2[2])+str(menu2[1])))
                        back()

                elif menu2[0] == '*':
                    print("REINICIANDO")
                    client.publish("sensor/huella/contraseña","$")
                    back()

        if menu == ['#', '3']:
            
            one='0'
            client.publish("sensor/huella/contraseña","3")
            time.sleep(0.5)
            submenu = True
            
            while submenu == True:

                if(get_adafruitio_message()=='1'):
                    one='1'
                    aio.send_data(password.key,one)
                    time.sleep(0.5)
                    client.publish("sensor/huella/contraseña",'?')
                    back()

                elif menu2[0] == '*':
                    print("REINICIANDO")
                    client.publish("sensor/huella/contraseña","$")
                    back()

        elif menu[0] != '#' and menu[1] == '#' and guardar_menu != str(get_adafruitio_message()):
            datos_simples = str(menu[0])
            aio.send_data(password.key,datos_simples)
            guardar_menu=menu[0]
        
        elif menu[0] != '#' and menu[1] != '#': 
            menu[1] = '#'
        
        elif menu[0] == '*':
            print("REINICIANDO")
            client.publish("sensor/huella/contraseña","$")
            back()


def on_message(client, userdata, msg):
    
    global num_celsius
    global num_humedad
    global numero_datos
    global suma_celsius
    global suma_humedad
    global guardar_celsius
    global unavez
    global fecha_hora
    numero_datos_celsius=60
    numero_datos_humedad=60
    
    if msg.topic == "sensor/DHT11/temperature_celsius":
        print (str(msg.payload))
        DatosCelsius = list_to_float_dato(str(msg.payload))
        fecha_hora = list_to_float_fecha_hora(str(msg.payload))
        
        print(f"\n\n\nCelsius: {DatosCelsius}")
        if (num_celsius <= numero_datos_celsius):
            guardar_celsius.append(DatosCelsius)
            suma_celsius = suma_celsius + DatosCelsius
            
            print(f"vector + posicion: {guardar_celsius[num_celsius]} {num_celsius}")
            print(f"suma total: {suma_celsius}")
            if(num_celsius>0):
                prom_celsius = (suma_celsius/(num_celsius+1))
                print(f"promedio total: {prom_celsius} \n\n\n")
            print(guardar_celsius)
            num_celsius = num_celsius + 1
        if (num_celsius > numero_datos_celsius):
            json_celsius.insert(0,"Dato: "+ str(prom_celsius)+";"+"Fecha y Hora:"+fecha_hora)
            with open("Celsius.json",'w') as outfile:
                json.dump(json_celsius,outfile,indent=4)
            aio.send_data(celsius.key, str(prom_celsius))
            num_celsius = 0
            prom_celsius = 0
            suma_celsius = 0   
        
    if msg.topic == "sensor/DHT11/humidity":
        DatosHumedad = list_to_float_dato(str(msg.payload))
        fecha_hora = list_to_float_fecha_hora(str(msg.payload))
        print(f"\n\n\nHumedad: {DatosHumedad}")
        if (num_humedad <= numero_datos_humedad):
            guardar_humedad.append(DatosHumedad)
            suma_humedad = suma_humedad + DatosHumedad
            
            print(f"vector + posicion: {guardar_humedad[num_humedad]} {num_humedad}")
            print(f"suma total: {suma_humedad}")
            if(num_humedad>0):
                prom_humedad = (suma_humedad/(num_humedad+1))
                print(f"promedio total: {prom_humedad} \n\n\n")
            print(guardar_humedad)
            num_humedad = num_humedad + 1
        if (num_humedad > numero_datos_humedad):
            json_humedad.insert(0,"Dato: "+ str(prom_humedad)+";"+"Fecha y Hora:"+fecha_hora)
            with open("Humedad.json",'w') as outfile:
                json.dump(json_humedad,outfile,indent=4)

            aio.send_data(humedad.key, str(DatosHumedad))
            num_humedad = 0
            prom_humedad = 0
            suma_humedad = 0 
   
    if msg.topic == "sensor/huella/submenu":
        dato_submenu=list_to_string(str(msg.payload)).strip("''")
        print(f"control: {dato_submenu}")
        '''with open("Humedad.json") as file:
                json_humedad = json.load(file)

        with open("Celsius.json",'w') as file:
            json_celsius[0] = json.load(file)'''
        if (unavez==0):
            print("UNAVEEEZZzzz")
            dato_submenu='0'
            print(dato_submenu)
        unavez=1

        if(str(dato_submenu)=='!0'):
            print(f"inicio: {dato_submenu}")
            print("configuracion 1: ", configuracion)
            bool_configuration_True()
            aio.send_data(password.key,'Ajustes de sensor de huella:\n 1. Registrar Huella\n 2. Eliminar Huella\n 3. Eliminar base de datos\nSeleccione la opcion y envie con #, utilice * para volver al inicio\n\n ')
        if(str(dato_submenu)=='!1'):
            print(f"inicio: {dato_submenu}")
            bool_submenu_True()
            aio.send_data(password.key,'Submenu 1:\nEscriba el número de identificación (del 1 al 99), utilice * para volver al inicio\n\n ')
        if(str(dato_submenu)=='!2'):
            print(f"inicio: {dato_submenu}")
            bool_submenu_True()
            aio.send_data(password.key,'Submenu 2:\nEscriba el número de identificación (del 1 al 99) que desea eliminar y envielo con #\n, utilice * para volver al inicio\n\n ')
        if(str(dato_submenu)=='!3'):
            print(f"inicio: {dato_submenu}")
            bool_submenu_True()
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
                aio.send_data(password.key,inicio[0])
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
    print("Se conecto con mqtt " + str(rc))
    client.subscribe("sensor/#")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(servidor, 1883)
client.loop_start()

with open("Humedad.json") as file:
    json_humedad = json.load(file)

with open("Celsius.json") as file:
    json_celsius = json.load(file)

while True:
    password_send()
    #client.publish("sensor/huella/contraseña","#")