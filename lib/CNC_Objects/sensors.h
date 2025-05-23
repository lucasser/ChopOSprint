/*Classes for different sensors
    CR Touch
        type: "crtouch"
        pwm:
        signal:

    limit switch:
        type: "limitSwitch"
        input:

    no sensor:
        type; "noSensor"


    To add more:

    class yourSensor : public Sensor;
    required functions:
        constructor - register pins, setup
        prep() - run before starting to level
        stow() - run after leveling, make sure to set detected to false
        toString() - print out sensor data
*/

#include <ESP32Servo.h>
#include <FunctionalInterrupt.h>

#ifndef Sensors_H
#define Sensors_H

//abstract sensor class
class Sensor {
    public:
        Sensor() {}; //default constructor
        virtual void prep() = 0; //run before starting to level
        virtual void stow() = 0; //run after leveling
        virtual String toString() = 0;
        bool detect() {return detected;}; //general check if sensor is triggered
        void IRAM_ATTR sensorIsr() {detected = true;}; //interrupt function for the sensor
    protected:
        bool detected = false; //switches to true when the sensor detects something.
};

//CRTouch/BLTouch sensor
class CRTouch : public Sensor {
    public:
        CRTouch(int pwm, int signal);
        void prep(); //registers interrupt, ejects probe
        void stow(); //removes interrupt, stows probe
        String toString(); //print out sensor data

    private:
        Servo sensor; //servo object for sensor control
        int control; //servo type control pin
        int input; //the input pin
};

//no level sensor (belt or extrusion).
//When calling detect() automatically returns true
class NoSensor : public Sensor {
    public:
        NoSensor();
        void prep() {return;} //not needed
        void stow() {return;} //not needed
        String toString(); //print out sensor data
};

//a simple limit switch.
class LimitSwitch : public Sensor {
    public:
        LimitSwitch(int signal);
        void prep(); //register interrupt
        void stow(); //remove interrupt
        String toString(); //print out sensor data
    private:
        int input; //the input pin
};

//[TODO]: something for extruder


#endif //sensors