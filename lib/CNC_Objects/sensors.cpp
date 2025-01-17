#include "sensors.h"

CRTouch::CRTouch(int pwm, int signal): control(pwm), input(signal) {
  sensor.attach(control);
  pinMode(input, INPUT_PULLUP);
  delay(100);
  sensor.write(160);
}


//[TODO]: lookup actual crtouch commands
void CRTouch::prep() {
  attachInterrupt(digitalPinToInterrupt(input), std::bind(&Sensor::sensorIsr, this), CHANGE);
  sensor.write(10);
}

void CRTouch::stow() {
  detected = false;
  sensor.write(90);
  detachInterrupt(digitalPinToInterrupt(input));
}

NoSensor::NoSensor() {
  detected = true;
}

LimitSwitch::LimitSwitch(int signal): input(signal) {}

void LimitSwitch::prep() {
  attachInterrupt(digitalPinToInterrupt(input), std::bind(&Sensor::sensorIsr, this), CHANGE);
}

void LimitSwitch::stow() {
  detected = false;
  detachInterrupt(digitalPinToInterrupt(input));
}