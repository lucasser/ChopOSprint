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
    required functions: constructor, registerInterrupt


*/

#include <ESP32Servo.h>

#ifndef Sensors_H
#define Sensors_H

//special interrupt function
//void IRAM_ATTR isr() {detected = true;}

//abstract sensor class
class Sensor {
    public:
        Sensor() {}; //default constructor
        virtual void prep() = 0; //run before starting to level
        virtual void stow() = 0; //run after leveling
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
};

//a simple limit switch.
class LimitSwitch : public Sensor {
    public:
        LimitSwitch();
        void prep(); //register interrupt
        void stow(); //remove interrupt
};

//[TODO]: something for extruder


#endif //sensors