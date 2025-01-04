/*
    Different leveling logic functions depending on the axis confuguration
*/

#include "axis.h"
#include "sensor.h"
#include <Arduino.h>

//level axis that has one probing location/no sensor
void level1pos(Axis* axis) {
    if (axis->levelSensor->detect()) {
        return;
    }
    axis->moveRelative(20);
    axis->levelSensor->prep();
    while (!axis->levelSensor->detect()) {
        axis->moveRelative(-1);
    }
    axis->stop();
}

//level axis that has 2 probing locations and axis communication is over RPI
void level2posSerial(Axis* axis) {

}

//level axis that has 2 probing locations and axis communication is inside esp32
void level2posLocal(Axis* axis) {

}