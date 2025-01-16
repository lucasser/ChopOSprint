# Communication protocol

## RPI -> esp

Command list:
Move
legal: m a x10, m a t10 x10

- Relative/absolute
- {Axis, Pos}
- time

Level (l)
only pass one axis and wait for response before continuing

- Axis

Toolhead (t) [TODO] not gonna do yet

- ?

Update settings (u)

- Path to setting
- Value

Stop (actually suspend) (s)

Resume (r)

- true/false

## Esp -> rpi

Move another axis
Finished leveling
Maybe some kind of response
