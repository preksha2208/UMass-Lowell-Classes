from flask import Flask, request, json
from flask_restful import Resource, Api
import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import time
from influxdb import InfluxDBClient
import datetime

app = Flask(__name__)
api = Api(app)


class HelloWorld(Resource):
    def post(self):
        value = request.get_data()
        value = json.loads(value)
        return {'hello': value['user']}
    def get(self):
        pass


api.add_resource(HelloWorld, '/test')

app.run(host='0.0.0.0', debug=True)

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

GPIO.setup(18, GPIO.OUT)


# Set up a client for InfluxDB
dbclient = InfluxDBClient('0.0.0.0', 8086, 'root', 'root', 'mydb')

broker_address="10.0.0.179"    #broker address (your pis ip address)

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
