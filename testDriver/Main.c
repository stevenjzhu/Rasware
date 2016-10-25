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
#define min 10.0f //distance from wall in cm
#define max 20.0f
#define leftMotorPin PIN_B2
#define rightMotorPin PIN_B1
#define leftSensorPin PIN_D0
#define rightSensorPin PIN_D1
#define blueLEDPin PIN_F2
#define greenLEDPin PIN_F3

//variables
float leftPower;
float rightPower;

void setup(void) {
	if (initialized) {
		return;
	  }

	initialized = true;

	rightPower = -1;
	leftMotor = InitializeServoMotor(leftMotorPin, false);
	rightMotor  = InitializeServoMotor(rightMotorPin, true);
	leftSensor = InitializeADC(leftSensorPin);
	rightSensor = InitializeADC(rightSensorPin);
	ADCReadContinuously(leftSensor,0.01);
	ADCReadContinuously(rightSensor,0.01);
}

int main(void) {
    // Initialization code can go here
	setup();

	while (1) {
		float current = 7.83/ADCRead(leftSensor)-1.66;
		SetMotor(leftMotor, 1);
        if(GetTimeUS()/1000){
            rightPower += 0.1;
        }
        SetMotor(rightMotor, rightPower);

        if(current > max){
            SetMotor(leftMotor, 0.5);
            SetMotor(rightMotor, 1);
        }

        // follow wall
        if(current < min){
            SetMotor(rightMotor, -1);
        }else{
            SetMotor(rightMotor, 1);
        }
        SetMotor(leftMotor, 1);
	}
}
