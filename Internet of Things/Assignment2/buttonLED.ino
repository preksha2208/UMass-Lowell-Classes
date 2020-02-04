/* 
   Code taken from:
   ledon.ino from https://github.com/Daniel-Pivonka/iot/blob/master/spr2020/class2/examples/ledon/ledon.ino
   bouncebutton.ino from https://github.com/Daniel-Pivonka/iot/blob/master/spr2020/class2/examples/bouncebutton/bouncebutton.ino
*/

#define BUTTON 4
#define LED 5
bool buttonstate;
bool ledOn = false;

void setup() {
  // put your setup code here, to run once:

  // setup serial connunication speed
  Serial.begin(115200);

  // setup pin as input
  pinMode(BUTTON, INPUT);

  // setup LED as output
  pinMode(LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  // read button state
  buttonstate = digitalRead(BUTTON);

  // if the button is clicked and the LED is not on, then turn on the LED
  if (buttonstate and not ledOn){
      digitalWrite(LED, HIGH);
      delay(500);
      // set the flag to indicate that the LED is turned on
      ledOn = true;
  }

  // if the button is clicked and the LED is currently on, then turn off the LED
  else if (buttonstate and ledOn){
    digitalWrite(LED, LOW);
    delay(500);
    // set flag to indicate that the LED is turned off
    ledOn = false;
  }
}
