#include <stdio.h>
#include <stdlib.h>
#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include <RASLib/inc/motor.h>
#include <RASLib/inc/adc.h>
#include <RASLib/inc/linesensor.h>

//define components
tBoolean initialized = false;
tMotor *leftMotor;
tMotor *rightMotor;
tADC *leftSensor;
tADC *rightSensor;
tLineSensor *lineSensor;

//define constants
#define wallDistance 15 //distance from wall in cm
#define leftMotorPin PIN_B2
#define rightMotorPin PIN_B1
#define leftSensorPin PIN_D0
#define rightSensorPin PIN_D1
#define blueLEDPin PIN_F2
#define greenLEDPin PIN_F3

//variables
float leftPosition;
float rightPosition;
float last; //indicates last distance from wall
int lastOp; //indicates last operation (0 for NULL, 1 for leftTurn,2 for rightTurn)

void setup(void) {
	if (initialized) {
		return;
	  }

	initialized = true;

	last = wallDistance;
	lastOp = 0;
	leftPosition = 1;
	rightPosition = 1;
	leftMotor = InitializeServoMotor(leftMotorPin, false);
	rightMotor  = InitializeServoMotor(rightMotorPin, true);
	leftSensor = InitializeADC(leftSensorPin);
	rightSensor = InitializeADC(rightSensorPin);
	ADCReadContinuously(leftSensor,0.01);
	ADCReadContinuously(rightSensor,0.01);
}

void leftTurn(float factor) {
	//Printf("Turning left ");
	leftPosition = 0.95-factor*0.90;
	rightPosition = 0.05+factor*0.90;
}

void rightTurn(float factor) {
	//Printf("Turning right ");
	leftPosition = 0.05+factor*0.90;
	rightPosition = 0.95-factor*0.90;
}

void wallAdjust(void) {
	float current = 7.83/ADCRead(leftSensor)-1.66;
	float factor = abs(current-wallDistance)/wallDistance;
	//Printf("%f ",factor);
	if (factor > 1) {factor = 1;}
//    Printf("%f,%f\n",current,ADCRead(leftSensor));

	if (abs(current-wallDistance) < 3) {
		leftPosition = 1.0f;
		rightPosition = 1.0f;
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


		SetMotor(leftMotor,leftPosition);
		SetMotor(rightMotor,rightPosition);
	}
}
