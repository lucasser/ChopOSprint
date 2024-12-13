#include "sensors.h"
#include <Arduino.h>
#include "ESP32Servo.h"



CRTouch::CRTouch() {
    sensor.attach(control);
    pinMode(input, INPUT_PULLUP);
}