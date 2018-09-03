# NMLB
*No Model Left Behind*

A DIY battery-powered lost-model alarm for RC drones and airplanes, based on the Attiny85 chip.

![alt tag](../master/NMLB_finished.jpg)

The project was inspired by the HellGate buzzer (http://buzzer.hellgatefpv.com/), which I suggest to buy if this DIY project seems too difficult.

A simpler version of this project can be build with a perf-board using through-hole components > [Through-hole version](../master/BUILD_noCH.md). Note that in this case the lipo-charging function cannot be done.

Usage
--------------

See the [Instructions](../master/Instructions.pdf) pdf file.


Firmware
--------------

The program to be compiled and written onto the attiny85 is '[NMLB_V.1.0.ino](../master/NMLB_V.1.0.ino)'.
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


The components are the following (for many I give an approximate range):


- Attiny85 in the SOIC package (ATTINY85-20SU or ATTINY85-20SH)
- TP4056 IC (https://tinyurl.com/ya2jh52k)
- Active Buzzer 5V. For the PCB I used the SMD active patch buzzer 9.5mm * 9.5mm * 5mm SMD one (https://tinyurl.com/ydh3g4ac)
- C1: 1-10 μF capacitor
- R1: 330-600Ω resistor
- R2: 20-33 kΩ resistor
- R3: 10 kΩ resistor
- R4: 50-100 kΩ resistor
- RPROG: 20 kΩ - 33 kΩ resistor for a charging current below 50mA.
- For the PCB the switch is the MSK-01A (https://tinyurl.com/y9ptwtey).
- I used 70mAh 1s lipo batteries (https://tinyurl.com/y8frzauv). Any other similar size will be OK.
