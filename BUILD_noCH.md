# NMLB - Through-hole version


A simpler version of this project can be build with a perf-board using through-hole components, without the lipo-charging function since that requires the surface-mounted IC TP4056.

Indeed, my first prototype was done in this way.
![alt tag](../master/NMLB_noCH_top.jpg)
![alt tag](../master/NMLB_noCH_bottom.jpg)

The schematic is also simpler:
![alt tag](../master/Schematic_NMLB_noCH-V.1.0.png)


The components are:

- Attiny85 in the 8-pin DIP package
- 8-pin DIP socket to mount the IC so that it can be removed easily (optional)
- Active Buzzer 5V. A typical buzzer for flight controllers is OK.
- C1: 1uF capacitor
- R1: 330 O resistor
- R2: 22 kO resistor
- R3: 10 kO resistor
- R4: 50 kO - 100 kOhm resistor
- A switch (optional) or a plug for the lipo. I used the MSK-01A (https://tinyurl.com/y9ptwtey).
- Small 1s Li-Po battery. I used a 70mAh one (https://tinyurl.com/y8frzauv).


