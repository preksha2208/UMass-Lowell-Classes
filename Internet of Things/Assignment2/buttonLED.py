import RPi.GPIO as GPIO
import time

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

GPIO.setup(21,GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(18, GPIO.OUT)

# booleans to keep track of whether the button has been pressed and whether the LED is currently on
buttonstate = False
ledOn = False

while True:
	buttonstate = GPIO.input(21)

	# turn LED on if the button is pressed and the LED is off
	if buttonstate == 1 and ledOn == 0:
		GPIO.output(18, GPIO.HIGH)
		time.sleep(.2)
		ledOn = True
	
	# keep LED if it is already on and the button isn't being pressed
	elif buttonstate == 0 and ledOn == 1:
		GPIO.output(18, GPIO.HIGH)
		time.sleep(.2)
	
	# turn LED off otherwise
	else:
		GPIO.output(18, GPIO.LOW)
		time.sleep(.2)
		ledOn = False

GPIO.cleanup()
