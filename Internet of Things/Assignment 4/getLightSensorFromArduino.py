import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import time
from influxdb import InfluxDBClient
import datetime

"""
Get light sensor value from arduino using mqtt
save all light sensor value to influxdb
query influxdb for average light sensor value from last ten seconds
if average light sensor value < 200, turn led on
else, turn led off
"""

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

GPIO.setup(18, GPIO.OUT)


# Set up a client for InfluxDB
dbclient = InfluxDBClient('0.0.0.0', 8086, 'root', 'root', 'mydb')

broker_address="10.0.0.179"    #broker address (your pis ip address)
lightstaten = 0
ledOn = 'off'

def on_message(client, userdata, message):
    global lightstate
    global ledOn
    lightstate = int.from_bytes(message.payload, byteorder='big')

    #get current time
    receiveTime=datetime.datetime.utcnow()
   #create json to insert into db
    json_body = [{
		"measurement": 'test',
		"time": receiveTime,
		"fields": {
		"lightstate": lightstate}}]
    
    dbclient.write_points(json_body)

    query = 'select mean("value") from "/lightstate" where "time" > now() - 10s'
    result = dbclient.query(query)

    try:
        light_avg = list(result.get_points(measurement='/lightstate'))[0]['mean']
        print (light_avg)
    except:
        #print 'exception'
        pass


    if (buttonstate == 'on' and ledOn == 'off'):
	    GPIO.output(18, GPIO.HIGH)
	    time.sleep(.1)
	    ledOn = 'on'

	# keep LED if it is already on and the button isn't being pressed
    elif (buttonstate == 'off' and ledOn == 'on'):
	    GPIO.output(18, GPIO.HIGH)
	    time.sleep(.1)

	# turn LED off otherwise
    else:
	    GPIO.output(18, GPIO.LOW)
	    time.sleep(.1)
	    ledOn = 'off'

client = mqtt.Client() #create new client instance
client.connect(broker_address) #connect to broker

client.on_message=on_message #set the on message function

client.subscribe("/led") #subscirbe to topic

client.loop_start() #start client
try:
	while True:
		pass

except KeyboardInterrupt:
	pass

client.loop_stop() #stop clientß




#write to db

print("Finished writing to InfluxDB")