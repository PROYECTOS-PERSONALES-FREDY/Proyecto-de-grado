# Simple example of sending and receiving values from Adafruit IO with the REST
# API client.
# Author: Tony Dicola, Justin Cooper

# Import Adafruit IO REST client.
from Adafruit_IO import Client, Feed, Data, RequestError
import datetime
import time
################pip3 install adafruit-io
# Set to your Adafruit IO key.
# Remember, your key is a secret,
# so make sure not to publish it when you publish this code!
ADAFRUIT_IO_KEY = 'aio_rTEy86Jd3JCL3A4sN0axA5rDPblM'

# Set to your Adafruit IO username.
# (go to https://accounts.adafruit.com to find your username)
ADAFRUIT_IO_USERNAME = 'FredyxD25'

# Create an instance of the REST client.
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)
try:
    teclado = aio.feeds('teclado')
except RequestError:
    feed = Feed(name="teclado")
    teclado = aio.create_feed(feed)

#
# Adding data
#
aio.append(teclado.key, 0)
'''
# works the same as send now
aio.send_data(teclado.key, 40)
# setup batch data with custom created_at values
yesterday = (datetime.datetime.today() - datetime.timedelta(1)).isoformat()
today = datetime.datetime.now().isoformat()
data_list = [Data(value=50, created_at=today), Data(value=33, created_at=yesterday)]
# send batch data
aio.send_batch_data(teclado.key, data_list)
'''
#
# Retrieving data
#
while True:
    data = aio.receive_next(teclado.key)
    print(data)

    data = aio.receive(teclado.key)
    print(data)

    data = aio.receive_previous(teclado.key)
    print(data)
    time.sleep(1)
