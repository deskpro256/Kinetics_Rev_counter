# Kinetics Rev counter
Custom designed counter for Kinetics Nail Systems

###### The problem:
For testing nail polish in the laboratory, there is a need to simulate wear of the nail polish material in time.

Kinetics needed a solution for repeatable friction setup for their nail polish degrading testing. Before this device was implemented, the tests had to be done by hand, which didn't net reproducable results, as the pressure on the test material couldn't always be the same.

This device saves time, because now there is a way to automate it.

Now the person only needs to set the wight and the amount of times to rub the material.

This device is based on an Atmel ATTINY84 on a custom PCB with an OLED display and some buttons for input.
The schematic and PCB were designed in EasyEDA. Project link.
https://easyeda.com/deskpro256/Kinetics_rev_counter

There are design files in the repository for manufacturing if needed.

###### How the device operates:

- The user first places the test material in it's specified place.
- Add the desired weight to the test
- Turn on the device with the power switch
- Using the potentiometer knob, set the amount of times the device needs to rub the material(100-5000x)
- When the desired set count is reached, press the *Start* button
- The device will start the motor which will push and pull the test material the set amount of times.
- When the count reaches 0, the device will make a sound indicating it has finished the task and stop the motor.
- If there is an emergency or a need to stop the test, pressing the *Stop* button will stop the device. The power switch can also be used to stop the device.
