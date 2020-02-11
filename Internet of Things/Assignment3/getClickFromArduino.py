import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import time

"""
subscribe to mqtt broker at /led
check whether the client has set "on", which is equivalent to a button click
adjust booleans accordingly and continue loop
"""

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

GPIO.setup(18, GPIO.OUT)

buttonstate = 'off'
ledOn = 'off'

broker_address="10.0.0.179"    #broker address (your pis ip address)


def on_message(client, userdata, message):
	print(message.topic + " " + str(message.payload)) #print incoming messages
	buttonstate = str(message.payload)
	print("Button state {}", buttonstate)
	
	if buttonstate == 'on' and ledOn == 'off':
		GPIO.output(18, GPIO.HIGH)
		time.sleep(.2)
		ledOn = 'on'
	
	# keep LED if it is already on and the button isn't being pressed
	elif buttonstate == 'off' and ledOn == 'on':
		GPIO.output(18, GPIO.HIGH)
		time.sleep(.2)
	
	# turn LED off otherwise
	else:
		GPIO.output(18, GPIO.LOW)
		time.sleep(.2)
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

client.loop_stop() #stop client