/*
ALL UNITS IN MM!!!
Header file for everything to do with stepper motors:
    Axis class for controlling multiple steppers together
    supporting struct to pass coordinates for next move
    [TODO]: move from mm to mm*10^5 to get rid of float
*/

#ifndef Axis_H
#define Axis_H

#include <BasicStepperDriver.h>
#include <deque>
#include "arduinoJsonChar.h"
#include <vector>
#include <sensors.h>
#include <memory>
#include "../../config/config.h"

using std::vector;
using std::deque;
using std::unique_ptr;

#define ALLMOTORS auto& i : motors //macro for iterating through all the motor instances in Axis motors vector. Use i to access each instance

/*[TODO]:
    detailed comments on all functions (see any cpp func for reference)
*/
class Axis {
    //stores move action data
    struct move {
        char type; //r for relative, a for absolute
        float dist; //mm
        float time; //seconds
        String toString() {
            String out = "{\n\ttype: ";
            out += type;
            out += "\n\tdist: ";
            out += dist;
            out += "\n\ttime: ";
            out += time;
            out += "\n}";
            return out;
        }
    };
    //public functions
    public:
        //default constructor
        Axis();
        //json object constructor
        Axis(JsonVariant config);
        //destructor to delete sensor pointer
        ~Axis();

        /**
         * @brief configure all axis motors and sensors from a JSON config file
         * @param config JSON config file, see config/README.md
        */
        void loadConfig(JsonVariant config);
        //tick all motors
        void tick();

        //move the axis. specify type with 'r', or 'a'
        void generalMove(Axis::move move);

        //wait for other axis to finish move
        void delay(float time);
        //level the axis using custom function
        void level(); //[TODO]: switch which axis, do leveling logic invoking sensors as needed
        /**
         * @brief Set absolute position of specified motor in axis to offset.
         * @param id Id of motor to zero. leave empty to zero all of them
         */
        void zero(int id = -1);

        /*
         * hard break. deletes move queue, stops axis immediately
         * use in all cases unless actually printing
        */
        void stop();

        /** @brief stops all movement
         *
         * Saves current position and stashes move queue.
         * Movement can be resumed from that position with the move queue, or move queue can be trashed.
         * if called while Axis is suspended returns void and does not stop;
         */
        void suspend();

        /// @brief resumes movement after a stop. If called without a stops returns void.
        /// @param trash if true, trashes move queue, if false, movement is resumed from where the stop was called.
        void resume(bool trash);

        //prints out info about the axis
        String toString();

    //helper functions
    private:
        //begins next move
        void startNextMove();
        //convert millimeter input into motor steps
        int mmToSteps(float mm) {
            return mm/stepLen;
        }
        //convert motor step amount input into mm distance
        float stepsToMM(int steps) {
            return steps*stepLen;
        }
        //register motor using json object
        void setupMotor(JsonVariant stepper);
        //add correct sensor type for leveling
        void setupSensor(JsonVariant sensor);

        //print functions
        String printState();
        String printMoves();

    //public variables
    public:
        //axis id
        char id;
        //is the axis configured
        bool init = false;
        //pointer to sensor
        Sensor* levelSensor;

    //internal state variables
    private:
        //stores a stepper driver object allong with position data.
        struct Stepper {
            long prevActionTime; //the time from which to measure the interval
            signed long stepsDone = 0; //used to calculate curPos
            long timeForNextAction; //when to take the next action to not use up CPU unessasaralily
            long stepCount = 0;
            signed short dir = 1; //direction of move
            double curPos = 0.0; //location of motor

            //Assigned on init
            int MOTORSTEPS = 200; //steps per revolution
            int direction = 1; //1 for forward, -1 for reverse

            //correctly register the move (absolute or relative)
            void beginMove(Axis::move move, Axis* axis) {
                float dist;
                if (move.type == 'r') {
                    dist = axis->mmToSteps(move.dist);
                } else if (move.type == 'a') {
                    dist = axis->mmToSteps(move.dist - curPos);
                }
                motor->startMove(direction*dist, move.time*1000000L);
                Serial.println("registered move: " + String(dist));
                prevActionTime = micros();
                timeForNextAction = 0;
                stepsDone = 0;
                dir = (dist > 0) ? 1 : -1;
            };

            //prints out all motor data
            String toString() {
                char pos[10];
                dtostrf(curPos, 3, 6, pos);
                String out = "{\n\tdirection: ";
                out += direction;
                out += "\n\tmicrostep: ";
                out += motor->getMicrostep();
                out += "\n\tstepsPerRev: ";
                out += MOTORSTEPS;
                out += "\n\tposition: ";
                out += pos;
                out += "\n}";
                return out;
            };

            BasicStepperDriver* motor;
        };

        //set through config//
        int maxSpeed = 60;
        float maxPos; //max absolute position of motor
        float stepLen = 0.2; //mm per step
        float offset = 0; //difference between sensor trigger and axis 0 location

        //keep track of dynamic axis state//
        int microstep = 1; //the microstepping situation

        void (*levelFunction)(Axis*);

        //used in tick function to start moves correctly
        long moveTime = 0; //how long to wait for next move
        float startTime = 0; //when move started

        //list of motors
        vector<Stepper*> motors;

        deque<move> moveCommands; //queue of move commands to execute
        move currentMove; //the move currently being executed

        //Suspend data//
        //positions that motors were stopped on
        vector<float> stopPos;

        bool suspended = false; //is the axis in suspend state
};

//contains 4 coordinates for 4 axis, and time for the move. Absolute positions only
struct moveCommand{
    moveCommand() {
        for (auto i : coords) {
            i = NAN;
        }
    };
    char type; //relative or absolute
    std::array<float, AXISAMOUNT> coords; //stores positions for a move. size configurable in config.h
    float time = 0;
    String toString() {
        String out = "time: ";
        out += time;
        out += ", type: ";
        out += type;
        out += ", coords: ";
        for (float x : coords) {
            out += x;
            out += ", ";
        }
        return out;
    }
};

#endif //axis