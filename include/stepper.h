/*
ALL UNITS IN MM!!!
Header file for everything to do with stepper motors:
    Custom stepper driver class that can convert distance to steps
    Axis class for controlling multiple steppers together
    supporting struct to pass coordinates for next move

*/

#ifndef Stepper_H
#define Stepper_H

#include "DRV8825.h"
#include "LinkedList.h"
#include <queue>
#include "ArduinoJson.h"
#include "sensors.h"


/*driver for individual motors
    driven using distance in millimeters as opposed to steps.
*/
class Stepper : public DRV8825{
    public:
        //manual constructor
        Stepper(int DIR, int STEPS, bool DIRECT = true); //[TODO]: pass some object that has all pins, direction, step length, steps per revolution, max pos, everything else
        //constructor using json object
        Stepper(JsonVariant stepper);
        //move to a coordinate
        void moveAbsolute(float pos, float time);
        //move a distance
        void moveRelative(float dist, float time);
        //set current position to zero
        void zero();
        //tick the motor (wait time is built in)
        void tick();

    private:

        struct move {
            float dist;
            float time;
        };
        float maxPos;
        float stepLen = 0.2; //not const to set direction
        int MOTORSTEPS = 200; //steps per revolution

        float curPos = 0; //actual location of motor
        float projPos = 0; //projected location of motor when the move queue gets evaluated

        //used in tick function to start moves correctly
        float delay = 0; //how long to wait for next move
        float startTime; //when move started

        std::queue<move> moveCommands; //queue of move commands to execute

        void startNextMove(); //begins next move
        int mmToSteps(float mm); //convert millimeter input into motor steps
};

class Axis {
    public:
        //default conssstructor
        Axis();
        //json object constructor
        Axis(JsonVariant config);
        //destructor to delete sensor pointer
        ~Axis();
        //tick all motors
        void tick();
        //move all motors to coordinate
        void moveAbsolute(float pos, float time);
        //move all motors a set distance
        void moveRelative(float pos, float time);
        //wait for other axis to finish move
        void delay(float time);
        //level the axis using custom function
        void level(); //[TODO]: switch which axis, do leveling logic invoking sensors as needed
        bool isEmpty() {return empty;}

    protected:
        bool empty = false;

    private:

        //pointer to sensor
        Sensor* levelSensor;

        //register motor using json object
        void setupMotor(JsonVariant stepper);
        //add correct sensor type for leveling
        void setupSensor(JsonVariant sensor);
        LinkedList<Stepper> axis = LinkedList<Stepper>();
};

//Empty axis placeholder
class NOAXIS : public Axis {
    public:
        //Empty axis placeholder
        NOAXIS() {empty = true;};
};

//contains 4 coordinates for 4 axis, and time for the move. Absolute positions only
struct moveCommand{
    std::array<float, 4> coords = {NAN, NAN, NAN, NAN}; //x, y, z, e. Add values for more axis
    float time;

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

#endif //stepper