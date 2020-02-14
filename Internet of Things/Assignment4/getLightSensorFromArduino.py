# Write your code here :-)
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

ledOn = 'off'

def on_message(client, userdata, message):
    global lightstate
    global ledOn
    lightstate = int((message.payload).decode("utf-8"))  # convert bytes -> string -> int

    #get current time
    receiveTime=datetime.datetime.utcnow()
    
   #create json to insert into db
    json_body = [{
		"measurement": 'test',
		"time": receiveTime,
		"fields": {
		"lightstate": lightstate}}]
    
    dbclient.write_points(json_body)

    query = 'SELECT MEAN("lightstate") from "test" where "time" > now() - 10s'
    result = dbclient.query(query)

    try:
        light_avg = list(result.get_points(measurement='test'))[0]['mean']
        print("average light value: ", light_avg)
        if (light_avg >= 200):
            GPIO.output(18, GPIO.LOW)  # turn LED off if light average over last 10 seconds is >= 200
        else:
            GPIO.output(18, GPIO.HIGH)  # otherwise turn on
            
    except:
        #print 'exception'
        pass

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
