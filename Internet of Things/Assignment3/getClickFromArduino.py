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


broker_address="10.0.0.179"    #broker address (your pis ip address)
buttonstate = 'off'
ledOn = 'off'

def on_message(client, userdata, message):
	global buttonstate
	global ledOn
	buttonstate = (message.payload).decode("utf-8")
	
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

client.loop_stop() #stop client