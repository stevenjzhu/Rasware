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

static float wallDistance; //distance from wall
float leftPosition;
float rightPosition;
float last; //indicates last distance from wall
int lastOp; //indicates last operation (0 for NULL, 1 for leftTurn,2 for rightTurn)

void setup(void) {
	if (initialized) {
		return;
	  }

	initialized = true;

	wallDistance = 15; 
	last = 15;
	lastOp = 0;
	leftPosition = 1;
	rightPosition = 0;
	leftServo = InitializeServo(PIN_B2);
	rightServo  = InitializeServo(PIN_B1);
	leftSensor = InitializeADC(PIN_D0);
	rightSensor = InitializeADC(PIN_D1);
}

void leftTurn(int factor) {
	leftPosition = factor*0.55;
	rightPosition = factor*0.05;
}

void rightTurn(int factor) {
	leftPosition = factor*0.95;
	rightPosition = factor*0.45;
}

void wallAdjust(void) {
    float current = 7.83/ADCRead(leftSensor)-1.66;
    float factor = abs(current-wallDistance)/wallDistance;
//    Printf("%f,%f\n",current,ADCRead(leftSensor));
    if (abs(current-wallDistance)<abs(last-wallDistance)) {
	if (lastOp = 1) {leftTurn(factor);}
	else {rightTurn(factor);}
    }
    else if (abs(current-wallDistance)>abs(last-wallDistance)) {
	if (lastOp = 1) {rightTurn(factor);}
	else {leftTurn(factor);}
    }
    else {
        leftPosition = 1.0f;
        rightPosition = 0.0f;
    }
    Printf("%f %f\n",leftPosition,rightPosition);
}

int main(void) {
    // Initialization code can go here
  setup();

  while (1) {
    wallAdjust();
    // Set servo speed
    SetServo(leftServo,leftPosition);
    SetServo(rightServo,rightPosition);
    //Wait(0.1);
  }
}
