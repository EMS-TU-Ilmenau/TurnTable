# Turntable
Construction of a low-cost turntable positioner including 3D-printable parts and software to control it.

![Construction](construction.jpeg)

## Construction
3D-printable parts and assembly can be found in the [construction](construction/) directory.

## Firmware
Quick-and-dirty mash-up of the [StepperDrive controller board](https://github.com/EMS-TU-Ilmenau/StepperDrive) and cheap e-Bike brushless motor controller (with hall sensor inputs), documented in the [firmware](firmware/) directory.

## Interface
Use the Python interface to talk to the firmware and do the steps to degree conversions in the [interface](interface/) directory.