#include "axis.h"
#include <leveling.h>
#include <DRV8825.h>

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
    id = config["id"];
    JsonObject  axis = config["axis"];
    maxPos = axis["maxpos"];
    stepLen = axis["steplen"];
    offset = axis["0offset"];
    microstep = axis["microstep"];
    maxSpeed = axis["maxspeed"];
    String level = axis["level"];

    if (level == "2Local") {
        levelFunction = &level2posLocal;
    } else if (level == "2Serial") {
        levelFunction = &level2posSerial;
    } else {
        levelFunction = &level1pos;
    }

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

    String driver = stepper["driver"];
    if (driver == "DRV8825") {
        if (stepper["pins"].size() == 2) {
            mot.motor = new DRV8825(mot.MOTORSTEPS, stepper["pins"][0], stepper["pins"][1]);
        } else {
            mot.motor = new DRV8825(mot.MOTORSTEPS, stepper["pins"][0], stepper["pins"][1], stepper["pins"][2], stepper["pins"][3], stepper["pins"][4]);
        }
    }
    //[TODO]: Add more supported drivers
    mot.motor->setRPM(maxSpeed);
    mot.motor->setMicrostep(microstep);
    motors.push_back(mot);
}

void Axis::setupSensor(JsonVariant sensor) {
    String sensorType = sensor["type"];
    if (sensorType == "crtouch") {
        levelSensor = new CRTouch(sensor["pwm"], sensor["signal"]);
    } else if (sensorType == "noSensor") {
        levelSensor = new NoSensor();
    } else if (sensorType == "limitSwitch") {
        levelSensor = new LimitSwitch(int(sensor["input"]));
    }
}

void Axis::tick() {
    if (!init) {return;}
    for (ALLMOTORS) {
        if (micros() >= i.timeForNextAction + i.prevActionTime) {
            i.timeForNextAction = i.motor->nextAction();
            i.prevActionTime = micros();
            i.curPos += stepsToMM(i.motor->getStepsCompleted() - i.stepsDone);
            i.stepsDone = i.motor->getStepsCompleted();
        }
    }
    if ((startTime + moveTime <= micros()) && !moveCommands.empty() && !suspended) {
        startNextMove();
    }
}

void Axis::generalMove(move move) {
    if (!init) {return;}
    if (suspended) {
        for (ALLMOTORS) {
            i.beginMove(move, this);
        }
    }
    moveCommands.push(move);
}

void Axis::delay(float time) {
    generalMove({'r', 0, time});
}

void Axis::level() {
    (*levelFunction)(this);
    Serial.println("leveled :D");
}

void Axis::zero(int id) {
    if (id == -1){
        for (ALLMOTORS) {
            i.curPos = offset;
        }
    } else if (id < motors.size()) {
        motors[id].curPos = offset;
    }
}

void Axis::stop() {
    for (ALLMOTORS) {
        i.motor->stop();
    }
    if (!suspended) {
        currentMove = {};
        moveCommands = {};
    }
}

void Axis::suspend() {
    if (suspended) {return;}
    suspended = true;
    stop();
    //if under suspend, just stop all motors, if not store the data
    stopPos = {};
    for (ALLMOTORS) {
        stopPos.push_back(i.curPos); //save stopped position
    }
}

void Axis::resume(bool restart) {
    if (!suspended) {return;}
    stop(); //make sure there's no movement;
    if (restart) { //clear and reset everything
        currentMove = {};
        moveCommands = {};
        stopPos = {};
        suspended = false;
    } else {
        std::queue<move> temp = moveCommands;
        moveCommands = {};
        moveCommands.push(currentMove);
        while (!temp.empty()) {
            moveCommands.push(temp.front());
            temp.pop();
        }
        int j = 0;
        for (ALLMOTORS) {
            i.beginMove({'a', stopPos.at(j), RESETTIME}, this);
            j++;
        }
        moveTime = 5000000L;
        startTime = micros();
        suspended = false;
    }
}

String Axis::toString() {
    String out = "{\n\tmaxpos: ";
    out += maxPos;
    out += "\n\t0offset: ";
    out += offset;
    out += "\n\tsteplen: ";
    out += stepLen;
    out += "\n\tmicrostep: ";
    out += microstep;
    out += "\n\tmaxspeed: ";
    out += maxSpeed;
    out += "\n}\nmotors: [";
    for (ALLMOTORS) {
        out += i.toString();
    }
    out += "]\nsensor: ";
    out += levelSensor->toString();
    out += "}";
    return out;
}

void Axis::startNextMove() {
    currentMove = moveCommands.front();
    moveCommands.pop();
    moveTime = currentMove.time*1000000L;
    startTime = micros();
    for (ALLMOTORS) {
        i.beginMove(currentMove, this);
    }
}