"""
use api to query db for light average and return that value with get
send 'on' or 'off' to esp or to pi broker to toggle respective LEDs
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
dbclient = InfluxDBClient('0.0.0.0', 8086, 'root', 'root', 'mydb')
GPIO.setup(21, GPIO.OUT)

client = mqtt.Client()  # create new client instance
client.connect(broker_address)  # connect to broker

app = Flask(__name__)
api = Api(app)


class light(Resource):
    def post(self):  # used to set the LED on either the pi or esp
        value = request.get_data()
        value = json.loads(value)

        if value['device'] == 'pi' and value['state'] == 'on':
            print("turning pi light on")
            client.publish("\pilight", "on")

        elif value['device'] == 'pi' and value['state'] == 'off':
            print("turning pi light off")
            client.publish("\pilight", "off")

        elif value['device'] == 'esp' and value['state'] == 'on':
            print("turning esp light on")
            client.publish("\led", "on")  # tell esp LED to turn on

        elif value['device'] == 'esp' and value['state'] == 'off':
            print("turning esp light off")
            client.publish("\led", "off")  # tell esp LED to turn off

        else:
            pass  # some other data was incorrectly passed in
        return

    def get(self): # returns the light average over last ten seconds
        query = 'SELECT MEAN("lightstate") from "test" where "time" > now() - 10s'
        result = dbclient.query(query)  # query database for light average
        # return light average
        return list(result.get_points(measurement='test'))[0]['mean']


api.add_resource(light, '/light')

app.run(host='0.0.0.0', debug=True)