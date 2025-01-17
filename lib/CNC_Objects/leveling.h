/*
    Different leveling logic functions depending on the axis confuguration
*/

#include "axis.h"
#include "sensor.h"
#include <Arduino.h>

//level axis that has one probing location/no sensor
//"1" in config
void level1pos(Axis* axis) {
    if (axis->levelSensor->detect()) {
        axis->zero();
        return;
    }
    axis->generalMove({'r', 10, 0});
    axis->levelSensor->prep();
    while (!axis->levelSensor->detect()) {
        axis->generalMove({'r', -1, 0});
    }
    axis->stop();
    axis->levelSensor->stow();
    axis->zero();
    axis->generalMove({'r', 10, 0});
    axis->levelSensor->prep();
    while (!axis->levelSensor->detect()) {
        axis->generalMove({'r', -1, 0.5}); //[TODO?]: adjust speed
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