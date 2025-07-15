#include "sensors.h"

CRTouch::CRTouch(int pwm, int signal): control(pwm), input(signal) {
  sensor.attach(control);
  pinMode(input, INPUT_PULLUP);
  sensor.write(160);
  delay(100);
}

//[TODO]: lookup actual crtouch commands
void CRTouch::prep() {
  attachInterrupt(digitalPinToInterrupt(input), std::bind(&Sensor::sensorIsr, this), CHANGE);
  sensor.write(10);
  delay(10);
}

void CRTouch::stow() {
  sensor.write(90);
  delay(10);
  detachInterrupt(digitalPinToInterrupt(input));
  detected = false;
}

String CRTouch::toString() {
  String out = "{\n\ttype: crtouch\n\tpwm: ";
  out += control;
  out += "\n\tsignal: ";
  out += input;
  out += "\n}";
  return out;
}

NoSensor::NoSensor() {
  detected = true;
}

String NoSensor::toString() {
  String out = "{\n\ttype: nosensor\n}";
  return out;
}

LimitSwitch::LimitSwitch(int signal): input(signal) {}

void LimitSwitch::prep() {
  attachInterrupt(digitalPinToInterrupt(input), std::bind(&Sensor::sensorIsr, this), CHANGE);
}

void LimitSwitch::stow() {
  detachInterrupt(digitalPinToInterrupt(input));
  detected = false;
}

String LimitSwitch::toString() {
  String out = "{\n\ttype: limitSwitch\n\tinput: ";
  out += input;
  out += "\n}";
  return out;
}
