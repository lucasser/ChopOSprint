/*
    Different leveling logic functions depending on the axis confuguration
    If calling a level function
*/

#include <axis.h>

//level axis that has one probing location/no sensor
//"1" in config
void level1pos(Axis* axis) {
    Serial.println("1 leveled");
    if (axis->levelSensor->detect()) {
        axis->zero();
        return;
    }
    axis->generalMove({'r', 10, 0});
    axis->levelSensor->prep();
    axis->generalMove({'r', -1000, 0});
    while (!axis->levelSensor->detect()) {
        axis->tick(); //keep ticking the axis until the sensor is triggered
    }
    axis->stop();
    axis->levelSensor->stow();
    axis->zero();
    axis->generalMove({'r', 10, 0});
    axis->levelSensor->prep();
    axis->generalMove({'r', -1000, 500}); //[TODO?]: adjust speed
    while (!axis->levelSensor->detect()) {
        axis->tick(); //keep ticking the axis until the sensor is triggered
    }
    axis->stop();
    axis->levelSensor->stow();
    axis->zero();
}

//level axis that has 2 probing locations and axis communication is over RPI
//"2serial" in config
void level2posSerial(Axis* axis) {
    Serial.println("2Serial leveled");
}

//level axis that has 2 probing locations and axis communication is inside esp32
//"2local" in config
void level2posLocal(Axis* axis) {
    Serial.println("2Local leveled");
}