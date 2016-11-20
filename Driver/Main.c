#include <stdio.h>
#include <stdlib.h>
#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include <RASLib/inc/motor.h>
#include <RASLib/inc/adc.h>
#include <RASLib/inc/linesensor.h>
#include <RASLib/inc/servo.h>

//define components
tBoolean initialized = false;
tMotor *leftMotor;
tMotor *rightMotor;
tServo *gateServo;
tADC *leftSensor;
tADC *rightSensor;
tLineSensor *lineSensor;

//define pins
#define gateServoPin PIN_B2
#define leftMotorPin PIN_B6
#define rightMotorPin PIN_A4
#define leftSensorPin PIN_E0
#define rightSensorPin PIN_B5
#define blueLEDPin PIN_F2
#define greenLEDPin PIN_F3
#define linePin1 PIN_A7
#define linePin2 PIN_A6
#define linePin3 PIN_A5
#define linePin4 PIN_B4
#define linePin5 PIN_E5
#define linePin6 PIN_E4
#define linePin7 PIN_B1
#define linePin8 PIN_B0

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
		float left = ADCRead(leftSensor);
		float right = ADCRead(rightSensor);
		Printf("left: %f right: %f\n", left, right);
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
	rightSensor = InitializeADC(rightSensorPin);
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
