#include "axis.h"
#include <Arduino.h>
#include "leveling.h"

Axis::Axis() {}

Axis::Axis(JsonVariant config) {
    loadConfig(config);
}

//[TODO]: delete all pointers to motors in motors vector
Axis::~Axis() {
    delete levelSensor;
}

void Axis::loadConfig(JsonVariant config) {
    //axis
    JsonObject axis = config["axis"];
    maxPos = axis["maxpos"];
    stepLen = axis["steplen"];
    offset = axis["0offset"];
    microstep = axis["microstep"];
    maxSpeed = axis["maxspeed"];

    //motors
    JsonArray motors = config["motors"];
    for (JsonVariant i : motors) {
        setupMotor(i);
    }

    //sensor
    setupSensor(config["sensor"]);
    init = true;
}

//[TODO?]: add support for enable pin
void Axis::setupMotor(JsonVariant stepper) {
    Stepper mot;

    mot.MOTORSTEPS = stepper["stepsPerRev"];
    mot.direction = (stepper["direction"] == "rev") ? -1 : 1;

    char driver = stepper["driver"];
    switch (driver) {
        case 'DRV8825':
                if (stepper["pins"].size() == 2) {
                    mot.motor = new DRV8825(mot.MOTORSTEPS, stepper["pins"][0], stepper["pins"][1]);
                } else {
                    mot.motor = new DRV8825(mot.MOTORSTEPS, stepper["pins"][0], stepper["pins"][1], stepper["pins"][2], stepper["pins"][3], stepper["pins"][4]);
                }
            break;
        //[TODO]: Add more supported drivers
        default:
            return;
    }
    mot.motor->setRPM(maxSpeed);
    mot.motor->setMicrostep(microstep);
    motors.push_back(mot);
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
    for (ALLMOTORS) {
        if (micros() >= i.timeForNextAction + i.prevActionTime) {
            i.timeForNextAction = i.motor->nextAction();
            i.prevActionTime = micros();
            i.curPos += stepsToMM(i.motor->getStepsCompleted() - i.stepsDone);
            i.stepsDone = i.motor->getStepsCompleted();
        }
    }
    if ((startTime + moveTime <= micros()) && !moveCommands.empty()) {
        startNextMove();
    }
}

void Axis::moveAbsolute(float pos, float time) {
    moveCommands.push({'a', pos, time});
}

void Axis::moveRelative(float dist, float time) {
    moveCommands.push({'r', dist, time});
}

void Axis::delay(float time) {
    moveRelative(0, time);
}

void Axis::level() {
    /*
        correct level function
        zero();
        tell RPI im done?
    */
}

void Axis::zero(size_t id = -1) {
    if (id == -1){
        for (ALLMOTORS) {
            i.curPos = 0;
        }
    } else if (id < motors.size()) {
        motors[id].curPos = 0;
    }
}
/*
void Axis::stop() {
    if (suspend) {return;}
    suspendedMoves.push({curPos, 0});
    for (size_t i = 0; i < moveCommands.size(); i++) {
        suspendedMoves.push(moveCommands.front());
    }
    suspend = true;
}

void Axis::resume(bool restart) {
    if (restart) {
        suspendedMoves = {};
        suspend = false;
    } else {

    }
}*/

void Axis::startNextMove() {
    currentMove = moveCommands.front();
    moveCommands.pop();
    moveTime = currentMove.time*1000000L;
    startTime = micros();
    for (ALLMOTORS) {
        i.beginMove(currentMove, this);
    }
}