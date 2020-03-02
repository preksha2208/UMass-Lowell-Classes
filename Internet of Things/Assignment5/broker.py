"""
receive light sensor values from esp through mqtt and store in influxdb

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
global dbclient
dbclient = InfluxDBClient('0.0.0.0', 8086, 'root', 'root', 'mydb')

def on_message(client, userdata, message):  # what to do when get message from mqtt

    if message.topic == "/pilight" and (message.payload).decode("utf-8") == "on":
        print("received mqtt message")
        GPIO.output(21, GPIO.HIGH)  # turn on pi LED
        return
    elif message.topic == "/pilight" and (message.payload).decode("utf-8") == "off":
        print("received mqtt message")
        GPIO.output(21, GPIO.LOW)  # turn off pi LED
        return

    # check for messages that are intended for the esp
    if message.topic == "/led" and ((message.payload).decode("utf-8")== "on" or (message.payload).decode("utf-8") == "off"):
        return

    global lightstate

    # convert bytes -> string -> int
    lightstate = int((message.payload).decode("utf-8"))
    print("storing light state")
    # get current time
    receiveTime = datetime.datetime.utcnow()

   # create json to insert into db
    json_body = [{
        "measurement": 'test',
        "time": receiveTime,
        "fields": {
            "lightstate": lightstate}}]

    dbclient.write_points(json_body)


client = mqtt.Client()  # create new client instance
client.connect(broker_address)  # connect to broker


client.subscribe("/led")  # subscribe to topic
client.subscribe("/pilight")


client.on_message = on_message  # set the on message function


client.loop_start()  # start client
try:
    while True:
        pass

except KeyboardInterrupt:
    pass

client.loop_stop()  # stop clientß


# write to db

print("Finished writing to InfluxDB")
