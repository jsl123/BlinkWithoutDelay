#include <CmdParser.h>

/* Blink without Delay

 Turns on and off a light emitting diode(LED) connected to a digital
 pin, without using the delay() function.  This means that other code
 can run at the same time without being interrupted by the LED code.

 The circuit:
 * LED attached from pin 13 to ground.
 * Note: on most Arduinos, there is already an LED on the board
 that's attached to pin 13, so no hardware is needed for this example.


 created 2005
 by David A. Mellis
 modified 8 Feb 2010
 by Paul Stoffregen

 This example code is in the public domain.


 http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
 */

// constants won't change. Used here to set pin numbers:
// Pin 13: Arduino has an LED connected on pin 13
// Pin 11: Teensy 2.0 has the LED on pin 11
// Pin  6: Teensy++ 2.0 has the LED on pin 6
// Pin 13: Teensy 3.0 has the LED on pin 13
const int ledPin =  13;      // the number of the LED pin
boolean running = true;
CmdParser cmd(&pCB);

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long currentMillis;

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 200;           // interval at which to blink (milliseconds)

void(*resetFunc) (void) = 0;

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  Serial.begin (9600);
  while (!Serial) {
    ;
  }
  cmd.init(true);
}

void loop()
{
  // check to see if it's time to blink the LED; that is, if the
  // difference between the current time and last time you blinked
  // the LED is bigger than the interval at which you want to
  // blink the LED.
  currentMillis = millis();

  if (running && (currentMillis - previousMillis > interval)) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }

  cmd.loop();
}

int pCB(char *inBuffer, int length) {
  boolean setLed = false;
  int rc = 0;

  if (0 == strncmp(inBuffer, "stop", length)) {
    running = false;
    Serial.println("Stopped");
  }
  else if (0 == strncmp(inBuffer, "start", length)) {
    running = true;
    previousMillis = currentMillis;
    Serial.println("Started");
  }
  else if (0 == strncmp(inBuffer, "on", length)) {
    if (! running) {
      ledState = HIGH;
      setLed = true;
    }
  }
  else if (0 == strncmp(inBuffer, "off", length)) {
    if (! running) {
      ledState = LOW;
      setLed = true;
    }
  }
  else if (0 == strncmp(inBuffer, "rate ", min (5, length))) {
    int newInterval = atoi(inBuffer + 5);
    if (newInterval > 10 && newInterval <= 1000) {
      interval = newInterval;
      Serial.println("New rate set");
    }
    else {
      Serial.println("Rate out of bounds (10 < rate <= 1000)");
    }
  }
  else if (0 == strncmp(inBuffer, "reset", length) || 0 == strncmp (inBuffer, "reboot", length)) {
    Serial.flush();
    delay(2000);
    resetFunc();
  }
  else if (0 == strncmp(inBuffer, "version", length)) {
    Serial.println(cmd.version());
  }
  else {
    rc = 1;
  } 

  if (setLed) {
    digitalWrite(ledPin, ledState);
  }

  return rc;
}
