# PID
PID controller for Atmega32

<hr>

## Developers

+ [Omid RostamAbadi](https://github.com/omidrostamabadi)
+ [Sadra Sabouri](https://github.com/sadrasabouri)

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

## Main Process
P, I and D coefficients and set point are loaded into ROM by user and AVR will load them into it's memory after execution.

|   Value   | Place on ROM |
|:---------:|:------------:|
|     P     |      20      |
|     I     |      21      |
|     D     |      22      |
| Set Point |      23      |

Then after each iteration (taken each 20ms) the error will be calculated from bellow equation:

```
error = error - pv;
```
(that `pv` stands for present value of the system)

Then new output will be calculated by bellow equation:

```
output = k_p * error + k_i * integral / SCALE + k_d * derivative * SCALE;
```
(in that terms are representing P, I and D segments respectively)

The output value will be shown on a 10 pin compact LED (as there is in picture).


Also there is a timer which uses AVR's timer as a interrupt source and counts each second saving its content in bellow RAM bytes:

|   Value   | Place on ROM |
|:---------:|:------------:|
| Seconds   |     0x200    |
|  Minute   |     0x201    |
|  Hour     |     0x202    |
|    Day    |     0x203    |
|   Week    |     0x204    |
|   Month   |     0x205    |

These values will be updated in each second and there will be sent to `PORTD` as a big-endian manner after each 16 minutes.


## Develop

You should use Proteus 8 and WinAVR compiler so that you can open `main.c` code in `Source Code` tab by right click on ATMEGA32 and click on `Edit Source Code`.

## Files
This repository contain multiple files which is described here in details:

+ `test.c`
A C program for first test of the PID controller and check if the logic is right. To compile it you can simply run:

```
$ gcc test.c
```

+ `PID.pdsprj`
Main project file which contains both schematic and codes needed for project.

<i>Note: You should use Proteus 8.9 or bellow and WinAVR for compile.</i>
