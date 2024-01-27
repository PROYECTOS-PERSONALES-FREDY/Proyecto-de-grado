# https://www.youtube.com/watch?v=-0yz7YhHZvU&t=202s&ab_channel=E-math
# https://www.youtube.com/watch?v=NHRTC7iN9mI&ab_channel=UskoKruM2010
import json
#pip3 install adafruit-io
datos = {'Nombre': "David Nicanor", 'Correo': "david@hotmail.com", 'Numero telefonico': 22891011}

#forma1 escritura 
with open("david.json",'w') as outfile:
    json.dump(datos,outfile,indent=4)
print(datos)
#forma2 escritura
"""
e = open("david.json","w")
dato = json.dumps(datos,indent=4)
e.write(datos)
e.close()
"""
"""
#forma1 lectura
with open("david.json") as file:
    data = json.load(file)

#forma2 lectura
"""
"""
l = open("david.json","r")
aux = l.read()
dic = json.loads(aux)
l.close()
"""

"""
r = leer
r+ = leer y escribir
w = escribir, crear y truncar (eliminar contenido del archivo)
posicion del puntero del archivon al final
a = escribir y crear

a+ = leer, escribir y crear
"""