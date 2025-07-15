#include "axis.h"
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
	leveler = getLeveler(level, this);

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
    if (startTime + moveTime <= micros()) {
		moving = false;
        currentMove = {};
    }
	if (leveling) {
		leveling = !leveler->tick();
	}
    if (!moving && !moveCommands.empty() && !suspended && !leveling) {
        startNextMove();
    }
}

void Axis::generalMove(move move) {
    Serial.print("genMove: ");
    Serial.println(move.dist, 5);
    if (!init) {return;}
    if (suspended) {
		if (!moving) {
			moveMotors(move);
		}
    } else {
        moveCommands.push_back(move);
    }
}

void Axis::delay(float time) {
    generalMove({'r', 0, time});
}

void Axis::level() {
	leveling = true;
    Serial.println("started level");
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
	moving = false;
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
    stopPos = motors.at(0)->curPos;
    Serial.println("Suspend: saved: " + String(stopPos));
}

void Axis::resume(bool restart) {
    if (!suspended) {return;}
    stop(); //make sure there's no movement;
    if (restart) { //clear and reset everything
        currentMove = {};
        moveCommands = {};
        stopPos = {};
        suspended = false;
        moveTime = 0;
        startTime = 0;
    } else {
		moveMotors({'a', stopPos, RESETTIME});
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
    out += "\n\tsaved position: ";
    out += stopPos;
    out += "\n}";
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
	moveMotors(currentMove);
}

void Axis::moveMotors(Axis::move move) {
	int steps = 0;
	moving = true;

	for (ALLMOTORS) {
		if (move.type == 'r') {
			steps = mmToSteps(move.dist);
		} else if (move.type == 'a') {
			steps = mmToSteps(move.dist - i->curPos);
		}
		i->beginMove(steps, move.time);
	}

	//Set timing control for move
	moveTime = move.time*1000000L;
	startTime = micros();
	if (move.time == 0) {
		moveTime = motors.at(0)->motor->getTimeForMove(steps);
	}

}
