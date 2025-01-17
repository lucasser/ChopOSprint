# Printer Config

[TODO]: specify leveling function inside config
multiple driver support

This is the holding folder for configs that are not used at the moment. Use this folder to store configs for your printer (they will not be uploaded with the code). When uploading code to the ESPs, paste the appropriate json file into [config.json](/data/config.json)

JSON template:

```json
{
    "axis": [
        {
            "id": ,
            "axis": {
                "maxpos": ,
                "0offset": ,
                "steplen": ,
                "microstep": ,
                "maxspeed": ,
                "level":
            },
            "motors": [{
                "driver": ,
                "pins": ,
                "direction": ,
                "stepsPerRev":
            }],
            "sensor": {sensor specific info}
        }
    ]
}
```

- **Axis**: an array of axis for the esp to control. Can be any length, as long as the esp has the pins
- **ID**: the axis id. Can be any char as long as it is in [config.h](config.h), and the amount is saved to AXISAMOUNT
- **Maxpos**: the maximum position the axis can be at.
- **0offset**: difference in millimeters between sensor 0 position and axis 0 position. Can be changed during use through command (not yet implemented)
- **Steplen**: how many steps it takes to move a millimeter based on your axis drive method
- **Microstep**: The microstep resolution in use
- **maxspeed**: Maximum speed of the axis. Used if a time for a move is not given.
- **Level**: The leveling function to use. See [level.h](/lib/CNC_Objects/leveling.h)
- **Motors**: an array of stepper motors that drive the axis. Can be any length
- **Driver**: The type of motor driver in use
  - [x] DRV8825
  - **Pins**: array of 2 pins that connect to the motor driver [direction, step] or array of 5 pins [direction, step, microstep1, microstep2, microstep3]
- **Direction**: which direction the motor considers forward. Can be fwd or rev. Fwd is counterclockwise
- **StepsPerRev**: how many steps the motor needs to complete a revolution
- **Sensor**: the data for the leveling sensor. Supported sensors:
  - [x] CRTouch
  - [x] limit switch
  - [x] no sensor.

See [sensors](/lib/CNC_Objects/sensors.h) for sensor specific data

Sample config file:

```json
{
    "axis": [
        {
            "id":"x",
            "axis": {
                "maxpos": 400,
                "0offset": 0,
                "steplen": 0.2,
                "microstep": 1,
                "maxspeed": 240
            },
            "motors": [{
                "driver": "DRV8825",
                "pins": [2,15],
                "direction": "fwd",
                "stepsPerRev": 200
            }],
            "sensor": {
                "type": "limitSwitch",
                "input": 12
            }
        }
    ]
}
```
