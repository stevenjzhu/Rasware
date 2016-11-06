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
tLineSensor *lineSensor;

//define constants
#define leftMotorPin PIN_B2
#define rightMotorPin PIN_B1
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
    greenLight();

    while(1) {
    	int current = getCurrent();

	    switch(current){
	    	case 0:  //turn right
	    		while (!(current == 3 || current == 4)) {
		    		SetMotor(leftMotor,0.5);
		    		SetMotor(rightMotor,-0.2);
		    		current = getCurrent();
		    		Printf("x0");
		    	}
	    		break;
	    	case 1:
	    		SetMotor(leftMotor,0.5);
	    		SetMotor(rightMotor,0);
	    		Printf("x1");
	    		break;
	    	case 2:
	    		SetMotor(leftMotor,0.5);
	    		SetMotor(rightMotor,0.2);\
	    		Printf("x2");
	    		break;
	    	case 3:
	    		SetMotor(leftMotor,0.5);
	    		SetMotor(rightMotor,0.5);
	    		Printf("x3");
	    		break;
	    	case 4:
	    		SetMotor(leftMotor,0.5);
	    		SetMotor(rightMotor,0.5);
	    		Printf("x4");
	    		break;
	    	case 5:
	    		SetMotor(leftMotor,0.2);
	    		SetMotor(rightMotor,0.5);
	    		Printf("x5");
	    		break;
	    	case 6:
	    		SetMotor(leftMotor,0);
	    		SetMotor(rightMotor,0.5);
	    		Printf("x6");
	    		break;
	    	case 7:  //turn left
	    		while (!(current == 3 || current == 4)) {
		    		SetMotor(leftMotor,-0.2);
		    		SetMotor(rightMotor,0.5);
		    		current = getCurrent();
		    		Printf("x7");
		    	}
	    		break;
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

    for (i = 0; i < 8; i++) {
        if (line[i] > 0.3) {
        	total += i;
        	count += 1;
        	lineFound = true;	        
        }
    }

    if (lineFound) {current = total/count;}
    Printf("%d\n",current);
    return current;
}

void setup(void) {
	if (initialized) {
		return;
	}

	initialized = true;

	leftMotor = InitializeServoMotor(leftMotorPin, false);
	rightMotor  = InitializeServoMotor(rightMotorPin, true);
	SetMotor(leftMotor,0.5);
	SetMotor(rightMotor,0.5);
	lineSensor = InitializeGPIOLineSensor(
		PIN_B3,
		PIN_C4,
		PIN_C5,
		PIN_C6,
		PIN_C7,
		PIN_D6,
		PIN_D7,
		PIN_F4); //right to left
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
