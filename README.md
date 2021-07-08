# PID
PID controller for Atmega32

<hr>

<img src="https://github.com/sadrasabouri/PID/blob/master/OtherFiles/main.PNG">

PID controller details have been came bellow:
+ Changeable coefficients for `P`, `I` and `D`
+ Each coefficient should be edited by user between `0` and `16`
+ Working point can be set by user from `0` to `2.56` Volts.
+ Sampling time should be `20ms`
+ Input signal has a amplitude between `0` and `2.56` Volts.
+ Output is a 8 bit number on Port C showing a value between `0` - `2.56` Volts
+ When Atmega32 is not working on PID coefficients it work as a timer and increase a value which is on `200H` to `205H` of RAM.
+ When system starts it'll be reset and counter will become 0
+ After 16 mins 5 byte of counter will be sent to Port D in a little-endian mode.

## Develop

You should use Proteus 8 and WinAVR compiler so that you can open `main.c` code in `Source Code` tab by right click on ATMEGA32 and click on `Edit Source Code`.

