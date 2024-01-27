import paho.mqtt.client as mqtt
broker="192.168.219.100"
port=1883

def on_publish(client,userdata,result):             #create function for callback
    print("data published \n")
    pass

client= mqtt.Client("control1")                           #create client object

client.on_publish = on_publish                          #assign function to callback

client.connect(broker,port)                                 #establish connection

ret= client.publish("house/bulb1","on")                   #publish