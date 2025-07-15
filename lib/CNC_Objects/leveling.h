#ifndef Level_H
#define Level_H

/*
    Different leveling logic functions depending on the axis confuguration
    If calling a level function
*/

#include <sensors.h>

class Axis; //forward declaration

class Leveler {
    public:
        Leveler(Axis* axis) : axis(axis) {};
        virtual bool step() = 0; //level the axis using custom function
        virtual bool tick() = 0; //tick leveler
    protected:
		Axis* axis; //pointer to the axis being leveled
        int state = 0; //switches to true when the sensor detects something.
};

//CRTouch/BLTouch sensor
class OnePosLeveler : public Leveler {
    public:
        OnePosLeveler(Axis* axis): Leveler(axis) {}; //default constructor
        bool step() override; //level the axis using custom function
        bool tick() override; //tick leveler
};

Leveler* getLeveler(String type, Axis* axis); //get the correct leveler object based on the type

#endif //Level_H

////level axis that has one probing location/no sensor
////"1" in config
//void level1pos(Axis* axis) {
//    Serial.println("1 leveled");
//    if (axis->levelSensor->detect()) {
//        axis->zero();
//        return;
//    }
//    axis->suspend();
//    axis->generalMove({'r', 20, 0.25});
//    Serial.println(axis->getCurrentMove().dist);
//    while (axis->getCurrentMove().dist) {
//        axis->tick();
//    }
//    axis->levelSensor->prep();
//    delay(100);
//    axis->generalMove({'r', -1000, 0});
//    while (!axis->levelSensor->detect()) {
//        axis->tick();
//    }
//    axis->stop();
//    axis->levelSensor->stow();
//    axis->zero();
//    delay(100);
//    axis->generalMove({'r', 20, 0.25});
//    Serial.println(axis->getCurrentMove().dist);
//    while (axis->getCurrentMove().dist) {
//        axis->tick();
//    }
//    axis->levelSensor->prep();
//    delay(100);
//    axis->generalMove({'r', -1000, 300}); //[TODO?]: adjust speed
//    while (!axis->levelSensor->detect()) {
//        axis->tick();
//    }
//    axis->stop();
//    axis->levelSensor->stow();
//    axis->zero();
//    axis->resume(true);
//    Serial.println(axis->levelSensor->detect());
//}

////level axis that has 2 probing locations and axis communication is over RPI
////"2serial" in config
//void level2posSerial(Axis* axis) {
//    Serial.println("2Serial leveled");
//}

////level axis that has 2 probing locations and axis communication is inside esp32
////"2local" in config
//void level2posLocal(Axis* axis) {
//    Serial.println("2Local leveled");
//}