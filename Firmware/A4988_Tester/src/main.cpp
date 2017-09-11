/*
 * Microstepping demo
 *
 * This requires that M0, M1 be connected in addition to STEP,DIR
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>
#include "DRV8834.h"
#include "A4988.h"
#include "DRV8825.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200

// All the wires needed for full functionality
#define DIR 4
#define STEP 3
#define ENBL 2

// 2-wire basic config, microstepping is hardwired on the driver
// BasicStepperDriver stepper(DIR, STEP);

// microstep control for DRV8834
//#define M0 10
//#define M1 11
//DRV8834 stepper(MOTOR_STEPS, DIR, STEP, M0, M1);
// DRV8834 stepper(MOTOR_STEPS, DIR, STEP, ENBL, M0, M1);

// microstep control for A4988
 #define MS1 7
 #define MS2 8
 #define MS3 9
 A4988 stepper(MOTOR_STEPS, DIR, STEP, ENBL, MS1, MS2, MS3);

// microstep control for DRV8825
// same pinout as A4988, different pin names, supports 32 microsteps
// #define MODE0 10
// #define MODE1 11
// #define MODE2 12
// DRV8825 stepper(MOTOR_STEPS, DIR, STEP, MODE0, MODE1, MODE2);

void setup() {
    // setup serial port
    Serial.begin(115200);
    Serial.print("Welcome to the stepper tester! :D");

    /*
     * Set target motor RPM.
     * These motors can do up to about 200rpm.
     * Too high will result in a high pitched whine and the motor does not move.
     */
    stepper.begin(120);
}

void loop() {
    delay(1000);

    /*
     * Moving motor at full speed is simple:
     */
    stepper.setMicrostep(1); // make sure we are in full speed mode

    Serial.println("Starting loop, clockwise, 1Rev, 1MS");
    // these two are equivalent: 180 degrees is 100 steps in full speed mode
    stepper.move(100);
    stepper.rotate(180);

    Serial.println("1Rev Counter Clockwise");
    // one full reverse rotation
    stepper.move(-100);
    stepper.rotate(-180);

    /*
     * Microstepping mode: 1,2,4,8,16 or 32(DRV8834 only)
     * Mode 1 is full speed.
     * Mode 32 is 32 microsteps per step.
     * The motor should rotate just as fast (set RPM),
     * but movement precision is increased.
     */
    stepper.setMicrostep(8);

    Serial.println("8MS 1Rev Clockwise");
    // 180 degrees now takes 100 * 8 microsteps
    stepper.move(100*8);
    stepper.rotate(180);

    Serial.println("1Rev Counter Clockwise");
    // as you can see, using degrees is easier
    stepper.move(-100*8);
    stepper.rotate(-180);

}
