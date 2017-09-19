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
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

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
#define btn1    10

// Neo Pixel

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
// Color definitions
uint32_t	BLACK;
uint32_t	BLUE;
uint32_t	RED;
uint32_t	GREEN;
uint32_t  CYAN;
uint32_t  MAGENTA;
uint32_t  YELLOW;
uint32_t  WHITE;

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

    pinMode(btn1, INPUT_PULLUP);

    BLACK       = strip.Color(0,0,0);
    BLUE        = strip.Color(0,0,255);
    RED         = strip.Color(255,0,0);
    GREEN       = strip.Color(0,255,0);
    CYAN        = strip.Color(0,255,255);
    MAGENTA     = strip.Color(255,0,255);
    YELLOW      = strip.Color(255,255,0);
    WHITE       = strip.Color(255,255,255);


    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setPixelColor(0, 255, 0, 0);
    strip.show();
    delay(100);
    strip.setPixelColor(0, 0, 255, 0);
    strip.show();
    delay(100);
    strip.setPixelColor(0, 0, 0, 255);
    strip.show();
    delay(100);
    strip.setBrightness(64);
    strip.setPixelColor(0, YELLOW);
    strip.show();

    // setup serial port
    Serial.begin(115200);
    Serial.println("Welcome to the stepper tester! :D");
    Serial.println("Constant 16 step mode test");
    Serial.println();

    /*
     * Set target motor RPM.
     * These motors can do up to about 200rpm.
     * Too high will result in a high pitched whine and the motor does not move.
     */
    stepper.begin(120);
    stepper.setMicrostep(16); // make sure we are in full speed mode
    stepper.disable();
}

void loop() {
    bool flagPass = true ; // innocent until proven guilty!

    strip.setPixelColor(0, MAGENTA);
    strip.show();
    Serial.println("Ready for Testing!");
    Serial.println("Press button or send 's' to start test...");

    while((digitalRead(btn1)) && (Serial.available() == 0)){
      // do nothing / twiddle thumbs!
    }

    // turn motor on
    stepper.enable();

    strip.setPixelColor(0, BLUE);
    strip.show();
    delay(50);

    while(flagPass){
      // step forward 1 rotation
      for(int i = 0 ; i < 200 ; i++){
        // check for interrupt from button
        if(!(digitalRead(btn1))){
          flagPass = false; // time to exit...
        }
        if(!flagPass){
          break; // get out of for loop
        }
        stepper.move(1);
      }

      // step back 1 rotation
      for(int i = 0 ; i < 200 ; i++){
        // check for interrupt from button
        if(!(digitalRead(btn1))){
          flagPass = false; // time to exit...
        }
        if(!flagPass){
          break; // get out of for loop
        }
        stepper.move(-1);
      }
    }
    
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
