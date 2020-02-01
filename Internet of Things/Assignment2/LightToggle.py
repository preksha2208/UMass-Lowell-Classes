# This program toggles the LED on/off when the button is clicked

import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)

#setup pin as input
GPIO.setup(18, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(23, GPIO.OUT)

lightOn = False

while True:

    buttonState = GPIO.input(18)
    
    if (buttonState and not lightOn):
        lightOn = True
        GPIO.output(23, GPIO.HIGH)   # turn LED on

    if (not buttonState and lightOn):
        lightOn = False
        GPIO.out(23, GPIO.LOW)      # turn LED off