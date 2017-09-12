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

#include <stdio.h>

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

//PinDefines for reading stepper outputs
#define pin1A   A0
#define pin1B   A1
#define pin2A   A2
#define pin2B   A3

// Button input
#define btn1    A4

/*****************************************************************************/
// Global Vars
int   s1A;
int   s1B;
int   s1;
int   s2A;
int   s2B;
int   s2;
int   encbyte;

int   i = 0;

/*****************************************************************************/
// Function prototypes
void checkDriverOutputs(void);
void printDriverOStates(void);

void setup() {
    // setup pin inputs
    pinMode(pin1A, INPUT);
    pinMode(pin1B, INPUT);
    pinMode(pin2A, INPUT);
    pinMode(pin2B, INPUT);

    pinMode(btn1, INPUT);

    // setup serial port
    Serial.begin(115200);
    Serial.print("Welcome to the stepper tester! :D");

    /*
     * Set target motor RPM.
     * These motors can do up to about 200rpm.
     * Too high will result in a high pitched whine and the motor does not move.
     */
    stepper.begin(120);
    stepper.setMicrostep(1); // make sure we are in full speed mode
}

void loop() {
    delay(1000);

    // Tests that we want to run:
    // 1 disable driver and check outputs low
    Serial.println("Test 1: Disabling driver and checking outputs");
    stepper.disable();
    delay(10); // wait a bit for outputs to settle
    checkDriverOutputs();
    printDriverOStates();

    if(s1A || s1B || s2A || s2B){// one of the states is high, we have a problem!
      Serial.println("Error! All states should be low");
      Serial.println("Likely we have a faulty output stage, or wiring");
      Serial.println();
    }else{
      Serial.println("Test 1: Complete");
      Serial.println();
    }

    delay(100);

    // 2 enable and check output works, see what state we are in?
    Serial.println("Test 2: Enabling driver and checking outputs");
    stepper.enable();
    delay(10); // wait a bit for outputs to settle

    delay(1000);
    checkDriverOutputs(); //0
    printDriverOStates();

    stepper.move(1);
    delay(1000);
    checkDriverOutputs(); //1
    printDriverOStates();

    stepper.move(1);
    delay(1000);
    checkDriverOutputs(); //2
    printDriverOStates();

    stepper.move(1);
    delay(1000);
    checkDriverOutputs(); //3
    printDriverOStates();

    stepper.move(1);
    delay(1000);
    checkDriverOutputs(); //0
    printDriverOStates();

    stepper.move(1);
    delay(1000);
    checkDriverOutputs(); //1
    printDriverOStates();

    Serial.println("Test 2: Now checking reverse");

    stepper.move(-1);
    checkDriverOutputs(); //3
    printDriverOStates();

    stepper.move(-1);
    checkDriverOutputs(); //2
    printDriverOStates();

    stepper.move(-1);
    checkDriverOutputs(); //1
    printDriverOStates();

    stepper.move(-1);
    checkDriverOutputs(); //0
    printDriverOStates();

    stepper.move(-1);
    checkDriverOutputs(); //3
    printDriverOStates();

    stepper.move(-1);
    checkDriverOutputs(); //2
    printDriverOStates();

    //TODO: Build driver state truth table

    Serial.println("Test 2: Complete");
    Serial.println();

    delay(100);

    // 3 step 4 times CW and check states
    Serial.println("Test 3: Do 4 full rotations");
    for(int l = 0 ; l<4 ; l++){
      stepper.rotate(360);
      stepper.rotate(-360);
    }

    Serial.println("Test 3: Complete");
    Serial.println();

    delay(100);


    // 4 repeat for 2, 4, 8, 16 MS
    Serial.println("Test 4: Change MS 2,4,8,16 and rotate");
    Serial.println("Test 4: 2 MicroSteps...");
    stepper.setMicrostep(2);
    for(int l = 0 ; l<4 ; l++){
      stepper.rotate(360);
      stepper.rotate(-360);
    }

    Serial.println("Test 4: 4 MicroSteps...");
    stepper.setMicrostep(4);
    for(int l = 0 ; l<4 ; l++){
      stepper.rotate(360);
      stepper.rotate(-360);
    }

    Serial.println("Test 4: 8 MicroSteps...");
    stepper.setMicrostep(8);
    for(int l = 0 ; l<4 ; l++){
      stepper.rotate(360);
      stepper.rotate(-360);
    }

    Serial.println("Test 4: 16 MicroSteps...");
    stepper.setMicrostep(16);
    for(int l = 0 ; l<4 ; l++){
      stepper.rotate(360);
      stepper.rotate(-360);
    }

    Serial.println("Test 4: Complete");
    Serial.println();

    delay(100);

    // 5 check states for above modes
    // 6 check states again
    // 7 disable, check states, try step

    Serial.println("Tests Complete! ");
    stepper.disable();
    delay(1000);

}

// function to check the driver outputs
void checkDriverOutputs(){
  delay(1); // wait for things to settle?
  s1A = digitalRead(pin1A);
  s1B = digitalRead(pin1B);
  s2A = digitalRead(pin2A);
  s2B = digitalRead(pin2B);

  // calc states?
  s1  = s1A - s1B;
  s2  = s2A - s2B;
  encbyte = (s1A << 3) | (s1B << 2) | (s2A << 1) | (s2B);
}

// function to print states of stepper outputs
void printDriverOStates(){
  char buffer [50];
  Serial.print("Driver Output States: ");
  i=sprintf (buffer, "2B:2A|1A:1B = %d%d%d%d  | %d | %d %d", s2B,s2A,s1A,s1B,encbyte,s1,s2);
  for(int l= 0; l<=i; l++)
  Serial.print(buffer[l]);
  Serial.println();
}
