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

void leftTurn(float factor) {
	//Printf("Turning left ");
	leftPosition =  0.95-factor*0.40;
	rightPosition = 0.45-factor*0.40;
}

void rightTurn(float factor) {
	//Printf("Turning right ");
	leftPosition = 0.55+factor*0.40;
	rightPosition = 0.05+factor*0.40;
}

void wallAdjust(void) {
	float current = 7.83/ADCRead(leftSensor)-1.66;
	float factor = abs(current-wallDistance)/wallDistance;
	//Printf("%f ",factor);
	if (factor > 1) {factor = 1;}
//    Printf("%f,%f\n",current,ADCRead(leftSensor));

	if (current == wallDistance) {
		leftPosition = 1.0f;
		rightPosition = 0.0f;
    }
    else {
		if (abs(current-wallDistance)<abs(last-wallDistance)) {
			if (lastOp == 1) {
				leftTurn(factor);
				lastOp=1;
			}
			else {
				rightTurn(factor);
				lastOp=2;
			}
	    }
	    if (abs(current-wallDistance)>abs(last-wallDistance)) {
			if (lastOp == 1) {
				rightTurn(factor);
				lastOp=2;
			}
			else {
				leftTurn(factor);
				lastOp=1;
			}
	    }
	}

    last = current;
}

int main(void) {
    // Initialization code can go here
	setup();

	while (1) {
		wallAdjust();
		//Printf("%f,%f\n",leftPosition,rightPosition);


		SetServo(leftServo,leftPosition);
		SetServo(rightServo,rightPosition);
		Wait(0.1);
	}
}
