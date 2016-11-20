#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
#define linePin8 PIN_A7
#define linePin7 PIN_A6
#define linePin6 PIN_A5
#define linePin5 PIN_B4
#define linePin4 PIN_E5
#define linePin3 PIN_E4
#define linePin2 PIN_B1
#define linePin1 PIN_B0

void blueLight();
void greenLight();
void setup();
int getCurrent();
void leftTurn(float current);
void rightTurn();

int main(void) {
    // Initialization code can go here
    tBoolean closed = false;
	setup();
    greenLight();

    while(1) {
    	int current = getCurrent();

	    switch(current){
	    	case 0:  //turn right
	    		//blueLight();
	    		SetServo(gateServo,0.5);
	    		closed = true;
	    		while (!(current == 3 || current == 4)) {
		    		SetMotor(leftMotor,0.5);
		    		SetMotor(rightMotor,-0.2);
		    		current = getCurrent();
		    	}
		    	//greenLight();
	    		break;
	    	case 1:
	    		SetServo(gateServo,0.5);
	    		closed = true;
	    		SetMotor(leftMotor,0.5);
	    		SetMotor(rightMotor,0);
	    		break;
	    	case 2:
	    		SetMotor(leftMotor,0.5);
	    		SetMotor(rightMotor,0.2);
	    		break;
	    	case 3:
	    		SetMotor(leftMotor,0.5);
	    		SetMotor(rightMotor,0.5);
	    		break;
	    	case 4:
	    		SetMotor(leftMotor,0.5);
	    		SetMotor(rightMotor,0.5);
	    		break;
	    	case 5:
	    		SetMotor(leftMotor,0.2);
	    		SetMotor(rightMotor,0.5);
	    		break;
	    	case 6:
	    		SetServo(gateServo,0.5);
	    		closed = true;
	    		SetMotor(leftMotor,0);
	    		SetMotor(rightMotor,0.5);
	    		break;
	    	case 7:  //turn left
	    		//blueLight();
	    		SetServo(gateServo,0.5);
	    		closed = true;
	    		while (!(current == 3 || current == 4)) {
		    		SetMotor(leftMotor,-0.2);
		    		SetMotor(rightMotor,0.5);
		    		current = getCurrent();
		    	}
		    	//greenLight();
	    		break;
	    }
	    if (closed) {
	    	SetServo(gateServo,0);
	    }
	}
}

int getCurrent(void) {
	int count = 0;
	int total = 0;
	int i;
	int current = 3;
    float line[8];
    tBoolean lineFound = false;

	LineSensorReadArray(lineSensor, line);

    for (i = 0; i < 7; i++) {
        if (line[i] > 0.3) {
        	Printf("    %d    ",i);
        	total += i;
        	count += 1;
        	lineFound = true;	        
        }
    }
    Printf("\n");
    if (lineFound) {current = total/count;}
    return round(current);
}

void setup(void) {
	if (initialized) {
		return;
	}

	initialized = true;

	leftMotor = InitializeServoMotor(leftMotorPin, false);
	rightMotor  = InitializeServoMotor(rightMotorPin, true);
	gateServo = InitializeServo(gateServoPin);
	SetMotor(leftMotor,0.5);
	SetMotor(rightMotor,0.5);
	SetServo(gateServo,0);
	lineSensor = InitializeGPIOLineSensor(
		linePin8,
		linePin7,
		linePin6,
		linePin5,
		linePin4,
		linePin3,
		linePin2,
		linePin1); //right to left
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
