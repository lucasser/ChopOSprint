#include "sensors.h"
#include <Arduino.h>
#include "ESP32Servo.h"



CRTouch::CRTouch() {
    sensor.attach(control);
    pinMode(input, INPUT_PULLUP);
}


/*
cr touch sensor drive code
#include <Arduino.h>
#include "ESP32Servo.h"
//#include "Servo.h"

Servo myservo;  // create servo object to control a servo
int Touch = 0;
const int interruptPin = 12;  // White probe output wire to Digital pin 2

// void IRAM_ATTR isr() {
//   Serial.println("interrupt");
//   myservo.write(90);
// }

void setup() {
  Serial.begin(115200);
  myservo.attach(15);  // attaches the servo on pin 9 to the servo object
  pinMode(interruptPin, INPUT_PULLUP);
  //attachInterrupt(interruptPin, isr, CHANGE);  // off to on will interrupt & go to blink function
  delay(100);
  myservo.write(90);
  delay(100);
  myservo.write(160);
  delay(60);
  myservo.write(60);
  delay(10);
  myservo.write(90);
  delay(10);
  myservo.write(10);
  Serial.println("hello");
}

void loop() {
  if (digitalRead(interruptPin) == 0) {
    Serial.println("detect");
  }
}
*/