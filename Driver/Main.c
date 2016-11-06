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
tADC *frontSensor;
tLineSensor *lineSensor;

//define constants
#define min 0.7f //distance from wall in cm
#define wallMin 0.4f
#define max 0.4f
#define cornerMax 0.2f
#define leftMotorPin PIN_B2
#define rightMotorPin PIN_B1
#define leftSensorPin PIN_D0
#define frontSensorPin PIN_D1
#define blueLEDPin PIN_F2
#define greenLEDPin PIN_F3

void blueLight();
void greenLight();
void setup();
void leftTurn(float current);
void rightTurn();

int main(void) {
    // Initialization code can go here
	setup();

	while (1) {
		//distance = 7.83/ADCRead(Sensor)-1.66;
		float current = ADCRead(leftSensor);
		float front = ADCRead(frontSensor);


		if (current < cornerMax){
			leftTurn(current);
		}
		if (front > wallMin) {
			rightTurn();
		}

		/*while (current < max) {
			blueLight();
			SetMotor(leftMotor, 0.3);
			SetMotor(rightMotor,0.7);
			Wait(0.05);
			SetMotor(leftMotor, 0.7);
			SetMotor(rightMotor, 0.3);
			Wait(0.05);
			current = ADCRead(leftSensor);
		}

		while (current > min) {
			blueLight();
			SetMotor(leftMotor,0.7);
			SetMotor(rightMotor,0.3);
			Wait(0.05);
			SetMotor(leftMotor,0.3);
			SetMotor(rightMotor,0.7);
			Wait(0.05);
			current = ADCRead(leftSensor);
		}*/

		greenLight();
		SetMotor(leftMotor, 1);
		SetMotor(rightMotor, 1);
	}
}

void setup(void) {
	if (initialized) {
		return;
	}

	initialized = true;

	leftMotor = InitializeServoMotor(leftMotorPin, false);
	rightMotor  = InitializeServoMotor(rightMotorPin, true);
	leftSensor = InitializeADC(leftSensorPin);
	frontSensor = InitializeADC(frontSensorPin);
}

void leftTurn(float current) {
	blueLight();
	SetMotor(leftMotor, -0.1);
	SetMotor(rightMotor,1);
	Wait(1.45);
	SetMotor(leftMotor,1);
	/*while (current < cornerMax) {
		greenLight();
		current = ADCRead(leftSensor);
	}*/
	Wait(1.35);
}

void rightTurn(void) {
	blueLight();
	SetMotor(leftMotor, 1);
	SetMotor(rightMotor, -0.1);
	Wait(1.55);
}

void blueLight(void) {
	SetPin(blueLEDPin,1);
    SetPin(greenLEDPin,0);
}

void greenLight(void) {
	SetPin(blueLEDPin,0);
    SetPin(greenLEDPin,1);
}
