# Printer Config

This is the holding folder for configs that are not used at the moment. Use this folder to store configs for your printer (they will not be uploaded with the code). When uploading code to the ESPs, paste the appropriate json file into [config.json](/data/config.json)

JSON template:

```
{
    "axis": [
        {
            "id": ,
            "motors": [{
                "pins":,
                "direction":,
                "maxpos":,
                "steplen":,
                "stepsPerRev":
            }],
            "sensor": {sensor specific info}
        }
    ]
}
```

axis: an array of axis for the esp to control. Can be any length, as long as the esp has the pins

id: the axis id. current support for x, y, z, e. modify movecommand::coords in
[stepper.h](/include/stepper.h) and main::axismap and main::AXIS in [main.cpp](/src/main.cpp) to add more

motors: an array of stepper motors that drive the axis. Can be any length

pins: array of 2 pin that connect to the motor driver [direction, step]

direction: which direction the motor considers forward. Can be fwd or rev. Fwd is counterclockwise

maxpos: the maximum position the axis can be at.

steplen: how many steps it takes to move a millimeter based on your axis control method

stepsPerRev: how many steps the motor needs to complete a revolution

sensor: the data for the leveling sensor
supported sensors: CRTouch, limit switch, no sensor.
see [sensors](/include/sensors.h) for sensor specific data

sample config file:
```
{
    "axis": [
        {
            "id":"z",
            "motors": [{
                "pins": [2,15],
                "direction": "fwd",
                "maxpos": 400,
                "steplen": 0.2,
                "stepsPerRev": 200
            }, {
                "pins": [4,13],
                "direction": "rev",
                "maxpos": 400,
                "steplen": 0.2,
                "stepsPerRev": 200
            }],
            "sensor": {
                "type": "crtouch",
                "pwm": 15,
                "signal": 12
            }
        }
    ]
}
```