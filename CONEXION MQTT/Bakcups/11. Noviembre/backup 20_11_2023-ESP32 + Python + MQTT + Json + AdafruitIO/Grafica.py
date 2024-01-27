from serial import*
from matplotlib import*
import matplotlib.pyplot as plt 
import matplotlib.animation as animation 
from matplotlib.lines import Line2D
from time import*
import numpy as np
import pandas as pd
from gettext import npgettext

#py -m pip install pyserial

#serial=Serial('/dev/ttyACM0',9600)
serial=Serial('COM3',9600)
cadena=list

AnchoGrafica = 20

#vectores para guardar datos del sensor
v1=[]
v2=[]
v3=[]
v4=[]

#Graficas
fig=plt.figure()

ax1=fig.add_subplot(221)
ax2=fig.add_subplot(222)
ax3=fig.add_subplot(223) 
ax4=fig.add_subplot(224)

x=np.linspace(0,1,1)

x=[]

y1=[]
y2=[]
y3=[]
y4=[]

#Codigo de lectura
while True:
    def animacion(i):
        cadena=serial.readline().decode('utf-8')
        
        cadena=np.array(cadena.split(','))
        
        s1=cadena[0]
        s2=cadena[1]
        s3=cadena[2]
        s4=cadena[3]

        s1=float(s1)
        s2=float(s2)
        s3=float(s3)
        s4=float(s4)

        v1.insert(0,s1)
        v2.insert(0,s2)
        v3.insert(0,s3)
        v4.insert(0,s4)

        if (len(v1)>AnchoGrafica):
            v1.pop()
            v2.pop()
            v3.pop()
            v4.pop()
            
            data={'Velocidad':[v1[0],v1[1],v1[2],v1[3],v1[4],v1[5],v1[6],v1[7],v1[8],v1[9]],'RPM':[v2[0],v2[1],v2[2],v2[3],v2[4],v2[5],v2[6],v2[7],v2[8],v2[9]]}
            df=pd.DataFrame(data, columns=['Velocidad','RPM'])
            df.to_csv('D:/Datos Fredy/Trabajos U/Programacion/Python/10_Datos_Anenomemtro.csv', sep=';')

        #print(s1,s2,s3,s4)
        print(v1,'\n',v2,'\n',v3,'\n',v4,'\n')

        x.insert(0,i)

        y1.insert(0,s1)
        y2.insert(0,s2)
        y3.insert(0,s3)
        y4.insert(0,s4)


        #if (len(y1)>10):
        #    y1.pop()
        #    y2.pop()

        ax1.clear()
        ax1.plot(x[0:AnchoGrafica],y1[0:AnchoGrafica],'Red')

        ax2.clear()
        ax2.plot(x[0:AnchoGrafica],y2[0:AnchoGrafica])

        ax3.clear()
        ax3.plot(x[0:AnchoGrafica],y3[0:AnchoGrafica])

        ax4.clear()
        ax4.plot(x[0:AnchoGrafica],y4[0:AnchoGrafica])

        '''
        ax1.title.set_text('Velocidad')
        ax1.set_xlabel("Tiempo")
        ax1.set_ylabel("m/s")
        
        ax2.title.set_text('Revoluciones Por Minuto')
        ax2.set_xlabel("Tiempo")
        ax2.set_ylabel("RPM")
'''

    anim=animation.FuncAnimation(fig,animacion, interval=100)    
    plt.show()