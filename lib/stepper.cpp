#include "stepper.h"
#include <Arduino.h>

Stepper::Stepper(int DIR, int STEPS, bool DIRECT): DRV8825(200, DIR, STEPS) {
    if (!DIRECT) {
        stepLen *= -1;
    }
}

//[TODO]: add microstep pins
Stepper::Stepper(JsonVariant stepper): stepLen(stepper["steplen"]), maxPos(stepper["maxpos"]), MOTORSTEPS(stepper["stepsPerRev"]), DRV8825(MOTORSTEPS, stepper["pins"][0], stepper["pins"][1]) {
    if (stepper["direction"] == "rev") {
        stepLen *= -1;
    }
}

int Stepper::mmToSteps(float mm) {
    int steps = mm/stepLen;
    return steps;
}

void Stepper::moveAbsolute(float pos, float time) {
    float toMove = pos - projPos;
    moveRelative(toMove, time);
}

void Stepper::moveRelative(float dist, float time) {
    float steps = mmToSteps(dist);
    moveCommands.push({steps, time});
    projPos += dist;
}

void Stepper::zero() {
    curPos = 0;
}

void Stepper::tick() {
    nextAction();
    if ((startTime+delay <= micros()) && !moveCommands.empty()) {
        startNextMove();
    }
}

void Stepper::startNextMove() {
    move toMove = moveCommands.front();
    moveCommands.pop();
    curPos += toMove.dist;
    delay = toMove.time*1000000L;
    startTime = micros();
    startMove(toMove.dist, toMove.time*1000000L);
}



Axis::Axis() {}

Axis::Axis(JsonVariant config) {
    //motors
    JsonArray motors = config["motors"];
    for (JsonVariant i : motors) {
        setupMotor(i);
    }

    //sensor
}

Axis::~Axis() {
    delete levelSensor;
}

void Axis::setupMotor(JsonVariant stepper) {
    Stepper motor(stepper);
    axis.add(motor);
}

void Axis::setupSensor(JsonVariant sensor) {
    char sensorType = sensor["sensor"];
    switch (sensorType) {
        case 'crtouch':
            levelSensor = new CRTouch(sensor["pwm"], sensor["signal"]);
            break;
        case 'noSensor':
            levelSensor = new NoSensor();
            break;
        case 'limitSwitch':
            levelSensor = new LimitSwitch();
            break;
        default:
    }
}

void Axis::tick() {
    for (int i = 0; i < axis.size(); i++) {
        axis[i].tick();
    }
}

void Axis::moveAbsolute(float pos, float time) {
    for (int i = 0; i < axis.size(); i++) {
        axis[i].moveAbsolute(pos, time);
    }
}

void Axis::moveRelative(float pos, float time) {
    for (int i = 0; i < axis.size(); i++) {
        axis[i].moveRelative(pos, time);
    }
}

void Axis::delay(float time) {
    for (int i = 0; i < axis.size(); i++) {
        axis[i].moveRelative(0, time);
    }
}

void Axis::level() {
}