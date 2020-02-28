"""
receive light sensor values from arduino through mqtt
store those values in mqtt
create restful api
restful api responsible for querying database for light average and returning that value
restful api responsible for sending mqtt messages to arduino to turn on led

"""

from flask import Flask, request, json
from flask_restful import Resource, Api
import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import time
from influxdb import InfluxDBClient
import datetime


broker_address = "10.0.0.179"  # broker address (your pis ip address)

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

GPIO.setup(21, GPIO.OUT)

client = mqtt.Client()  # create new client instance
client.connect(broker_address)  # connect to broker

client.on_message = on_message  # set the on message function

client.subscribe("/led")  # subscribe to topic

app = Flask(__name__)
api = Api(app)


class HelloWorld(Resource):
    def post(self):
        value = request.get_data()
        value = json.loads(value)

        if value['device'] == 'pi' and value['state'] == 'on':
            GPIO.output(21, GPIO.HIGH)  # turn on pi LED

        elif value['device'] == 'pi' and value['state'] == 'off':
            GPIO.output(21, GPIO.LOW)   # turn off pi LED

        elif value['device'] == 'esp' and value['state'] == 'on':
            client.publish("\led", "on")  # tell esp LED to turn on 

        elif value['device'] == 'esp' and value['state'] == 'off':
            client.publish("\led", "off") # tell esp LED to turn off

        else:
            pass  # some other data was incorrectly passed in
        return

    def get(self):
        query = 'SELECT MEAN("lightstate") from "test" where "time" > now() - 10s'
        result = dbclient.query(query)  # query database for light average
        return list(result.get_points(measurement='test'))[0]['mean']  # return light average


api.add_resource(HelloWorld, '/test')

app.run(host='0.0.0.0', debug=True)




# Set up a client for InfluxDB
dbclient = InfluxDBClient('0.0.0.0', 8086, 'root', 'root', 'mydb')


def on_message(client, userdata, message):
    global lightstate

    # convert bytes -> string -> int
    lightstate = int((message.payload).decode("utf-8"))

    # get current time
    receiveTime = datetime.datetime.utcnow()

   # create json to insert into db
    json_body = [{
        "measurement": 'test',
        "time": receiveTime,
        "fields": {
            "lightstate": lightstate}}]

    dbclient.write_points(json_body)




client.loop_start()  # start client
try:
    while True:
        pass

except KeyboardInterrupt:
    pass

client.loop_stop()  # stop clientß


# write to db

print("Finished writing to InfluxDB")
