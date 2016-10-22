#include <stdio.h>
#include <stdlib.h>
#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include <RASLib/inc/servo.h>
#include <RASLib/inc/adc.h>
#include <RASLib/inc/linesensor.h>

static tBoolean initialized = false;
static tServo *leftServo;
static tServo *rightServo;
static tADC *leftSensor;
static tADC *rightSensor;
static tLineSensor *lineSensor;
static float wallDistance;

float leftPosition;
float rightPosition;

void setup(void) {
  if (initialized) {
    return;
  }

  initialized = true;

  wallDistance = 20; //distance from wall
  leftPosition = 1;
  rightPosition = 0;
  leftServo = InitializeServo(PIN_B2);
  rightServo  = InitializeServo(PIN_B1);
  leftSensor = InitializeADC(PIN_D0);
  rightSensor = InitializeADC(PIN_D1);
}

void wallAdjust(void) {
    float current = 10.4/ADCRead(leftSensor)-6.5;
    if (current > wallDistance) {
        leftPosition -= 0.02f;
        rightPosition -= 0.02f;
    }
    else if (current < wallDistance) {
        leftPosition += 0.02f;
        rightPosition += 0.02f;
    }
    else {
        leftPosition = 1;
        rightPosition = 0;
    }

    if (leftPosition > 1.0f) {
	leftPosition = 1.0f;
    }
    else if (leftPosition < 0.6f) {
        leftPosition = 0.6f;
    }
    if (rightPosition > 0.4f) {
	rightPosition = 0.4f;
    }
    else if (rightPosition < 0.0f) {
        rightPosition = 0.0f;
    }   
    Printf("%f %f\n",leftPosition,rightPosition);
/* if (ADCRead(leftSensor) < ADCRead(rightSensor)) {
    if (ADCRead(leftSensor) < wallDistance) {
      leftPosition += 0.01f;
    }
    else if (ADCRead(leftSensor) > wallDistance) {
      leftPosition -= 0.01f;
    }
    else {
      leftPosition = 0;
    }
  }
  else if (ADCRead(leftSensor) > ADCRead(rightSensor)) {
    if (ADCRead(rightSensor) < wallDistance) {
      rightPosition -= 0.01f;
    }
    else if (ADCRead(rightSensor) < wallDistance) {
      rightPosition += 0.01f;
    }
    else {
      rightPosition = 1;
    }
  }*/
}

int main(void) {
    // Initialization code can go here
  setup();

  while (1) {
    wallAdjust();
    // Set servo speed
    SetServo(leftServo,leftPosition);
    SetServo(rightServo,rightPosition);
   // Wait(0.25);
  }
}
