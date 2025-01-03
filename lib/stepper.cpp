#include "stepper.h"
#include <Arduino.h>
#include "leveling.h"
/*
Stepper::Stepper(int DIR, int STEPS, bool DIRECT): DRV8825(200, DIR, STEPS) {
    if (!DIRECT) {
        stepLen *= -1;
    }
}

//[TODO]: add microstep pins
Stepper::Stepper(JsonVariant stepper): stepLen(stepper["steplen"]), maxPos(stepper["maxpos"]), MOTORSTEPS(stepper["stepsPerRev"]) {
    JsonArray pins = stepper["pins"];
    if (pins.size() == 5) {
        DRV8825(MOTORSTEPS, pins[0], pins[1], pins[2], pins[3], pins[4]);
    } else {
        DRV8825(MOTORSTEPS, stepper["pins"][0], stepper["pins"][1]);
    }
    if (stepper["direction"] == "rev") {
        stepLen *= -1;
    }
}

void Stepper::moveRelative(float dist, float time) {
    float steps = mmToSteps(dist);
    moveCommands.push({steps, time});
    projPos += dist;
}
*/



Axis::Axis() {}

Axis::Axis(JsonVariant config) {
    loadConfig(config);
}

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

    //motors
    JsonArray motors = config["motors"];
    for (JsonVariant i : motors) {
        setupMotor(i);
    }

    //sensor
    setupSensor(config["sensor"]);
    init = true;
}
//[TODO(maybe)]: add support for enable pin
void Axis::setupMotor(JsonVariant stepper) {
    Stepper mot;

    mot.MOTORSTEPS = stepper["stepsPerRev"];
    mot.direction = (stepper["direction"] == "rev") ? -1 : 1;
    mot.MOTORSTEPS = stepper["stepsPerRev"];

    char driver = stepper["driver"];
    switch (driver) {
        case 'DRV8825':
                if (stepper["pins"] == 2) {
                    mot.motor = new DRV8825(mot.MOTORSTEPS, stepper["pins"][0], stepper["pins"][1]);
                } else {
                    mot.motor = new DRV8825(mot.MOTORSTEPS, stepper["pins"][0], stepper["pins"][1], stepper["pins"][2], stepper["pins"][3], stepper["pins"][4]);
                }
            break;
        //[TODO]: Add more supported drivers
        default:
            return;
    }

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
    nextAction();
    if ((startTime+delay <= micros()) && !moveCommands.empty()) {
        startNextMove();
    }
}

void Axis::moveAbsolute(float pos, float time) {
    for (auto i : motors) {
        i.moveAbsolute(pos, time);
    }
}

void Axis::moveRelative(float pos, float time) {
    for (auto i : motors) {
        i.moveRelative(pos, time);
    }
}

void Axis::delay(float time) {
    for (auto i : motors) {
        i.moveRelative(0, time);
    }
}

void Axis::level() {
    /*
        correct level function
        zero();
        tell RPI im done?
    */
}

void Axis::zero() {
    for (auto i : motors) {
        i.zero();
    }
}

void Axis::stop() {

}

void Axis::resume(bool restart) {
}

int Axis::mmToSteps(float mm) {
    int steps = mm/stepLen;
    return steps;
}

float Axis::stepsToMM(float mm) {
    return 0.0f;
}

void Axis::moveAbsolute(float pos, float time) {
    float toMove = pos - projPos;
    moveRelative(toMove, time);
}

void Axis::startNextMove() {
    move toMove = moveCommands.front();
    moveCommands.pop();
    curPos += toMove.dist;
    delay = toMove.time*1000000L;
    startTime = micros();
    startMove(toMove.dist, toMove.time*1000000L);
}