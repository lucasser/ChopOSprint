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

class Sensor {
    public:
        Sensor() {};
        virtual void registerInterrupt() = 0;
};


class CRTouch : public Sensor {
    public:
        CRTouch(int pwm, int signal); //[TODO]: init should get passed sensor pin, and servo pin
        void registerInterrupt();

    private:
        Servo sensor;
        int control; //servo type control pin
        int input; //the input pin
};

class NoSensor : public Sensor {
    public:
        NoSensor();
        void registerInterrupt();
};

class LimitSwitch : public Sensor {
    public:
        LimitSwitch();
        void registerInterrupt();
};

//[TODO]: something for extruder


#endif //sensors