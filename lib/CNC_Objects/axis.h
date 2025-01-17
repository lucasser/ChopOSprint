/*
ALL UNITS IN MM!!!
Header file for everything to do with stepper motors:
    Axis class for controlling multiple steppers together
    supporting struct to pass coordinates for next move

*/

#ifndef Axis_H
#define Axis_H

#include <BasicStepperDriver.h>
#include <queue>
#include "arduinoJsonChar.h"
#include <vector>
#include <sensors.h>
#include "../../config/config.h"

using std::vector;
using std::queue;

#define ALLMOTORS auto i : motors //macro for iterating through all the motor instances in Axis motors vector. Use i to access each instance

/*[TODO]:
    detailed comments on all functions (see any cpp func for reference)
*/
class Axis {
    //stores move action data
    struct move {
        char type; //r for relative, a for absolute
        float dist; //mm
        float time; //seconds
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

    //helper functions
    private:
        //begins next move
        void startNextMove();
        //convert millimeter input into motor steps
        int mmToSteps(float mm) {
            return mm/stepLen*microstep;
        }
        //convert motor step amount input into mm distance
        float stepsToMM(int steps) {
            return steps/microstep*stepLen;
        }
        //register motor using json object
        void setupMotor(JsonVariant stepper);
        //add correct sensor type for leveling
        void setupSensor(JsonVariant sensor);

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
            float prevActionTime; //the time from which to measure the interval
            float stepsDone = 0; //used to calculate curPos
            float timeForNextAction; //when to take the next action to not use up CPU unessasaralily
            float curPos; //location of motor
            int MOTORSTEPS = 200; //steps per revolution
            int direction = 1; //1 for forward, -1 for reverse

            //correctly register the move (absolute or relative)
            void beginMove(Axis::move move, Axis* axis) {
                switch (move.type) {
                case 'r':
                    motor->startMove(direction*axis->mmToSteps(move.dist), move.time*1000000L);
                    break;
                case 'a':
                    motor->startMove(direction*(axis->mmToSteps(move.dist) - curPos), move.time*1000000L);
                    break;
                default:
                    return;
                }
                prevActionTime = micros();
                timeForNextAction = 0;
                stepsDone = 0;
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
        float startTime; //when move started

        //list of motors
        vector<Stepper> motors;

        queue<move> moveCommands; //queue of move commands to execute
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
        out += ", coords: ";
        for (float x : coords) {
            out += x;
            out += ", ";
        }
        return out;
    }
};

#endif //axis