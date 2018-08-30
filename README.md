# NMLB
*No Model Left Behind*

A DIY battery-powered lost-model alarm for RC drones and airplanes, based on the Attiny85 chip.

![alt tag](../master/NMLB_finished.jpg)

The project was inspired by the HellGate buzzer (http://buzzer.hellgatefpv.com/), which I suggest to buy if this DIY project seems too difficult.

A simpler version of this project can be build with a perf-board using through-hole components > [Through-hole version](../master/BUILD_noCH.md). Note that in this case the lipo-charging function cannot be done.

Usage
--------------

It connects to the RC model via a +5V and a GND wire.
Optionally, it can be connected also to the BUZZER- pad of a flight controller, which allows to use it as a normal buzzer.

By modifying the `bz_mode` flag in the code to `const int bz_mode = 2;` the buzzer can instead be controlled via a PWM signal (buzzer on if PWM is above 1800) of a traditional receiver.


It has a general on-off switch, as well as a sleep mode with very low power consumption (~3μA). It also charges the battery when connected to an external +5V. This is achieved via the TP4056 chip. 

When the buzzer is turned on, it makes 1, 2, or 3 long beeps if the battery voltage is below 4.0, 3.8, or 3.5V, respectively. It then beeps and blinks twice to confirm start.

— *IDLE* mode (. 3s). When the buzzer is on but the RC model has no power, the buzzer blinks the LED once every 3 seconds. After one minute it goes into sleep mode.

— *SLEEP* mode. In this mode the buzzer consumes only ~3μA, allowing it to stay active for several months with the 70mAh lipo battery. Every 4 seconds it checks the status and then goes back to sleep.

— *FLY* mode (.. 3s). When the RC model is turned on, the buzzer makes a (long)-(short)-(long) beep and, after 5 seconds,  it arms the lost-model alarm function. In this mode it blinks the LED twice every 3 seconds.

— *ALARM* mode (.... 3s). When power is lost (after it was on for more than 5 seconds), the buzzer starts waiting for one minute. In this time it blinks the LED four times every 3 seconds.
After the minute is passed, it beeps a SOS (_ _ _   . . .   _ _ _) every 5 seconds until the internal battery is drained, or the model is found.

— In order to deactivate the alarm, or the wait period, one can either briefly reconnect power to the RC model (for less than 5 seconds), or simply turn the buzzer off via the switch.


Firmware
--------------

The program to be compiled and written onto the attiny85 is '[NMLB_V.1.0.ino](https://github.com/DavidMarzocca/NMLB/blob/master/NMLB_V.1.0.ino)'.
I use Arduino IDE and an Arduino UNO with a self-made programming board to install it.
See [here](https://www.instructables.com/id/Program-an-ATtiny-with-Arduino/) for instructions to build one.

The Attiny in SOIC format can be programmed using a '[SOIC8 SOP8 to DIP8 200mil Wide-body Seat Programmer Adapter Socket](https://tinyurl.com/y9wba9u6') or a '[SOIC8 SOP8 Test Clip in-circuit programmer](https://tinyurl.com/yawm7v8t)'.

Build
--------------

![alt tag](../master/Schematic_NMLB-V1.0.png)

The schematic describes the connections and the components required to build the buzzer.
This can be done in any way you wish: perf-board, self-printed PCB, or by ordering the PCB via a manifacturer using the gerber files attached to the project.

![alt tag](../master/NMLB_PCB_top.png)
![alt tag](../master/NMLB_PCB_bottom.png)

The latter uses SMD components in the 1206 package, to make it easier to solder by hand. It is possible also to use smaller packages (I had 0603 resistors available and used those).


The components are:

*Buzzer part:*
- Attiny85 in the SOIC package (ATTINY85-20SU or ATTINY85-20SH)
- Active Buzzer 5V. For the PCB I used the SMD active patch buzzer 9.5mm * 9.5mm * 5mm SMD one (https://tinyurl.com/ydh3g4ac)
- C1: 1uF capacitor
- R1: 330Ω resistor
- R2: 22 kΩ resistor
- R3: 10 kΩ resistor
- R4: 50 kΩ - 100 kΩ resistor
- For the PCB the switch is the MSK-01A (https://tinyurl.com/y9ptwtey).
- I used 70mAh 1s lipo batteries (https://tinyurl.com/y8frzauv). Any other similar size will be OK.

*Charging part:*
- TP4056 IC (https://tinyurl.com/ya2jh52k)
- RPROG: 20 kΩ - 33 kΩ resistor for a charging current below 50mA.

