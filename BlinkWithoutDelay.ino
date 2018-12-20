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

enum {CMD_NOT_FOUND, CMD_STOP, CMD_START, CMD_ON, CMD_OFF, CMD_RATE, CMD_VERSION, CMD_RESET} cmds;
struct cmd_table_t cmd_table[] = {
  {"stop", CmdParser::PARSER_WORD, CMD_STOP},
  {"start", CmdParser::PARSER_WORD, CMD_START},
  {"on", CmdParser::PARSER_WORD, CMD_ON},
  {"off", CmdParser::PARSER_WORD, CMD_OFF},
  {"rate", CmdParser::PARSER_WORD_INT, CMD_RATE},
  {"reset", CmdParser::PARSER_WORD, CMD_RESET},
  {"version", CmdParser::PARSER_WORD, CMD_VERSION},
  {"reboot", CmdParser::PARSER_WORD, CMD_RESET},
  {NULL, CmdParser::PARSER_END, 0}
};
CmdParser parser(&pCB, cmd_table);

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
  parser.init();
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

  parser.loop();
}

int pCB(int cmd, int integer1) {
  boolean setLed = false;
  int rc = 0;

  switch (cmd) {
    case CMD_STOP:
      running = false;
      Serial.println("Stopped");
      break;

    case CMD_START:
      running = true;
      previousMillis = currentMillis;
      Serial.println("Started");
      break;

    case CMD_ON:
      if (! running) {
        ledState = HIGH;
        setLed = true;
      }
      break;
      
    case CMD_OFF:
      if (! running) {
        ledState = LOW;
        setLed = true;
      }
      break;
      
    case CMD_RATE:
      if (integer1 > 10 && integer1 <= 1000) {
        interval = integer1;
        Serial.println("New rate set");
      }
      else {
        Serial.println("Rate out of bounds (10 < rate <= 1000)");
      }
      break;
 
    case CMD_RESET:
      Serial.flush();
      delay(2000);
      resetFunc();
      break;

    //case CMD_VERSION:
      Serial.println(parser.GetVersion());
      break;
      
    case CMD_NOT_FOUND: // fall-through
    default:
      Serial.println("Command not implemented");
      rc = 1;
      break;
  }

  if (setLed) {
    digitalWrite(ledPin, ledState);
  }

  return rc;
}
