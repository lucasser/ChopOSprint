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
    for (ALLMOTORS) {
        delete i->motor;
        delete i;
    }
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
    Stepper* mot = new Stepper();

    mot->MOTORSTEPS = stepper["stepsPerRev"];
    mot->direction = (stepper["direction"] == "rev") ? -1 : 1;

    String driver = stepper["driver"];
    if (driver == "DRV8825") {
        if (stepper["pins"].size() == 2) {
            mot->motor = new DRV8825(mot->MOTORSTEPS, stepper["pins"][0], stepper["pins"][1]);
        } else {
            mot->motor = new DRV8825(mot->MOTORSTEPS, stepper["pins"][0], stepper["pins"][1], stepper["pins"][2], stepper["pins"][3], stepper["pins"][4]);
        }
    }
    //[TODO]: Add more supported drivers
    mot->motor->setRPM(maxSpeed);
    mot->motor->setMicrostep(microstep);
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
        if (micros() >= i->timeForNextAction + i->prevActionTime && i->motor->getCurrentState() != 0) {
            i->timeForNextAction = i->motor->nextAction();
            i->prevActionTime = micros();
            i->curPos += i->dir * stepsToMM(i->motor->getStepsCompleted() - i->stepsDone);
            //(currentMove.dist > 0) ? stepsToMM(i.motor->getStepsCompleted() - i.stepsDone) : -1*stepsToMM(i.motor->getStepsCompleted() - i.stepsDone);
            i->stepsDone = i->motor->getStepsCompleted();
        }
    }
    if (motors.at(0)->motor->getCurrentState() == 0) {
        currentMove = {};
    }
    if ((startTime + moveTime <= micros()) && !moveCommands.empty() && !suspended) {
        startNextMove();
    }
}

void Axis::generalMove(move move) {
    Serial.print("genMove: ");
    Serial.println(move.dist, 5);
    if (!init) {return;}
    if (suspended) {
        for (ALLMOTORS) {
            if (i->motor->getCurrentState() == 0) {
                i->beginMove(move, this);
            }
        }
    } else {
        moveCommands.push_back(move);
    }
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
            i->curPos = offset;
        }
    } else if (id < motors.size()) {
        motors.at(id)->curPos = offset;
    }
}

void Axis::stop() {
    for (ALLMOTORS) {
        i->motor->stop();
    }
}

void Axis::suspend() {
    stop();
    if (suspended) {return;}
    //if under suspend, just stop all motors, if not store the data
    if (currentMove.type == 'r') {
        currentMove.dist -= motors.at(0)->curPos;
    }
    moveCommands.push_front(currentMove);
    Serial.println(printMoves());
    suspended = true;
    stopPos = {};
    for (ALLMOTORS) {
        stopPos.push_back(i->curPos); //save stopped position
        Serial.println("Suspend: saved: " + String(i->curPos));
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
        int j = 0;
        for (ALLMOTORS) {
            Serial.println("Resume: motor at: " + String(stopPos.at(j)));
            i->beginMove({'a', stopPos.at(j), RESETTIME}, this);
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
        out += i->toString();
    }
    out += "]\nsensor: ";
    out += levelSensor->toString();
    out += "\nstate: ";
    out += printState();
    out += "}";
    return out;
}

String Axis::printState() {
    String out = "{\n\t";
    out += (suspended) ? "suspended" : "active";
    out += "\n\tcurrent move: ";
    out += currentMove.toString();
    out += "\n\tmove queue: ";
    out += printMoves();
    out += "\n\tsaved position: {";
    for (auto i : stopPos) {
        out += "\n\t\t";
        out += i;
    }
    out += "\n}}";
    return out;
}

String Axis::printMoves() {
    String out = "{";
    deque<move> temp = moveCommands;
    while (!temp.empty()) {
        out += temp.front().toString();
        temp.pop_front();
        Serial.println("loop");
    }
    out += "}";
    return out;
}

void Axis::startNextMove() {
    Serial.println("startNextMove");
    currentMove = moveCommands.front();
    moveCommands.pop_front();
    moveTime = currentMove.time*1000000L;
    startTime = micros();
    for (ALLMOTORS) {
        i->beginMove(currentMove, this);
    }
}