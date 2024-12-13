/*Classes for different sensors
    CR Touch



*/


#ifndef Sensors_H
#define Sensors_H

class CRTouch {
    public:
        CRTouch(); //[TODO]: init should get passed sensor pin, and servo pin


    private:
        Servo sensor;
        int control; //servo type control pin
        int input; //the input pin
};


#endif //sensors