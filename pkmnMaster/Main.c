#include <stdio.h>
#include <stdlib.h>
#include </home/mixs/ras/Rasware/RASLib/inc/common.h>
#include </home/mixs/ras/Rasware/RASLib/inc/gpio.h>
#include </home/mixs/ras/Rasware/RASLib/inc/time.h>
#include </home/mixs/ras/Rasware/RASLib/inc/motor.h>
#include </home/mixs/ras/Rasware/RASLib/inc/adc.h>
#include </home/mixs/ras/Rasware/RASLib/inc/linesensor.h>

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
#define distanceOfTheWall 0.8 // TODO
#define leftMotorPin PIN_B6
#define rightMotorPin PIN_A4
#define leftSensorPin PIN_E0
#define rightSensorPin PIN_E5
#define blueLEDPin PIN_F2
#define greenLEDPin PIN_F3
#define lineSensorList PIN_A7, \
		PIN_A6, \
		PIN_A5, \
		PIN_B4, \
		PIN_E5, \
		PIN_E4, \
		PIN_B1, \
		PIN_B0  //right to left, line sensor pins
#define nLineSensor 7
#define gateServoPin PIN_B2

//variables
tBoolean driveLeft;

void setup();
tBoolean isBlack(float line);
int getLine();
float getDistance(tADC *sensorPin);
tBoolean isLineSplit();
tBoolean isLineThick();
tBoolean isTheWall();
void walkLine();
void turnAround();
void walkForward(tBoolean onForward);
void grabBall();
void grabSideBall(tBoolean onLeft);
void turn90Degree(tBoolean onForward, tBoolean onLeft);
void reportZone(int zone);

int main(void) {
    // Initialization code can go here
// white -> red -> yellow -> green -> light blue -> blue -> purple -> black
    reportZone(0); // black
	setup();
	// walkForward(true);

	// zone1
	reportZone(1); // green
	while(!isLineThick()) {
		SetMotor(leftMotor, 1);
		SetMotor(rightMotor, 1);
	}
	walkForward(true);

	// zone2
	reportZone(2); // blue
	while(!isLineSplit()) {
		walkLine();
	}
	turnAround();
	walkForward(true);

	// zone3
	reportZone(3); // light blue
	while(!isLineThick()) {
		walkLine();
	}
	walkForward(true);

	// zone4
	reportZone(4); // black
	while(!isLineThick()) {
		walkLine();
	}
	walkForward(true);

	// zone5
	reportZone(5); // green
	while(!isTheWall()) {
		walkLine();
	}
	walkForward(true);

	// zone6
	reportZone(6); // blue
	grabSideBall(driveLeft);

	// zone7
	reportZone(7); // light blue
	while(!isLineSplit()) {
		walkLine();
	}
	turn90Degree(true, driveLeft);
	walkForward(true);

	// zone8
	reportZone(8); // black
	while(true) { // TODO, what to detect the end?
		walkLine();
	}

	// zonex
	reportZone(9); // green
	while(true) {
		// TBD, grab pokeballs continuously?
	}
}

void setup(void) {
	if (initialized) {
		return;
	  }

	initialized = true;

	driveLeft = true; // TODO, setting with built-in button?

	leftMotor = InitializeServoMotor(leftMotorPin, false);
	rightMotor  = InitializeServoMotor(rightMotorPin, true);
	leftSensor = InitializeADC(leftSensorPin);
	rightSensor = InitializeADC(rightSensorPin);
	ADCReadContinuously(leftSensor,0.01);
	ADCReadContinuously(rightSensor,0.01);
	lineSensor = InitializeGPIOLineSensor(lineSensorList);
}

tBoolean isBlack(float line) {
	return line > 0.3;
}

int getLine(void) { // TODO
	int count = 0;
	int total = 0;
	int i;
	int current = 3;
    float line[8];
    tBoolean lineFound = false;

	LineSensorReadArray(lineSensor, line);

    for (i = 0; i < nLineSensor; i++) {
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

float getDistance(tADC *sensorPin) {
	// return 7.83/ADCRead(sensorPin)-1.66;
	return 1/ADCRead(sensorPin);
}

tBoolean isLineSplit(void) {
	float line[8];
	int i, state = 0;

	LineSensorReadArray(lineSensor, line);

	for(i=0; i<nLineSensor; i++) {
		if(state%2 == 0 && isBlack(line[i]))
			state++;
		else if(state%2 == 1 && !isBlack(line[i]))
			state++;
	}
	return state > 2; // more than 2 splited black boxes
}

tBoolean isLineThick() {
	float line[8];
	int i, cnt = 0;

	LineSensorReadArray(lineSensor, line);

	for(i=0; i<nLineSensor; i++) {
		if(isBlack(line[i]))
			cnt++;
	}
	return cnt > 2; // more than 2 splited black boxes
}

tBoolean isTheWall() {
	tADC *focusSensor;
	if(driveLeft)
		focusSensor = leftSensor;
	else
		focusSensor = rightSensor;
	return getDistance(focusSensor) < distanceOfTheWall;
}

void walkLine(void) { // TODO
	int current = getLine();

    switch(current){
    	case 0:  //turn right
    		while (!(current == 3 || current == 4)) {
	    		SetMotor(leftMotor,0.5);
	    		SetMotor(rightMotor,-0.2);
	    		current = getLine();
	    		// Printf("x0");
	    	}
    		break;
    	case 1:
    		SetMotor(leftMotor,0.5);
    		SetMotor(rightMotor,0);
    		// Printf("x1");
    		break;
    	case 2:
    		SetMotor(leftMotor,0.5);
    		SetMotor(rightMotor,0.2);
    		// Printf("x2");
    		break;
    	case 3:
    		SetMotor(leftMotor,0.5);
    		SetMotor(rightMotor,0.5);
    		// Printf("x3");
    		break;
    	case 4:
    		SetMotor(leftMotor,0.5);
    		SetMotor(rightMotor,0.5);
    		// Printf("x4");
    		break;
    	case 5:
    		SetMotor(leftMotor,0.2);
    		SetMotor(rightMotor,0.5);
    		// Printf("x5");
    		break;
    	case 6:
    		SetMotor(leftMotor,0);
    		SetMotor(rightMotor,0.5);
    		// Printf("x6");
    		break;
    	case 7:  //turn left
    		while (!(current == 3 || current == 4)) {
	    		SetMotor(leftMotor,-0.2);
	    		SetMotor(rightMotor,0.5);
	    		current = getLine();
	    		Printf("x7");
	    	}
    		break;
    }
}

void turnAround(void) {
	SetMotor(leftMotor, -1);
	SetMotor(rightMotor,1);
	Wait(2.90);
	SetMotor(leftMotor, 0);
	SetMotor(rightMotor,0);
}

void walkForward(tBoolean onForward) {
	SetMotor(leftMotor, 1);
	SetMotor(rightMotor,1);
	Wait(0.6);
	SetMotor(leftMotor, 0);
	SetMotor(rightMotor,0);
}

void grabBall() {
	// TODO
}

void grabSideBall(tBoolean onLeft) {
	turn90Degree(true, onLeft);
	grabBall();
	turn90Degree(false, onLeft);
}

void turn90Degree(tBoolean onForward, tBoolean onLeft) {
	float leftMotorPower = onLeft?-0.1:1;
	float rightMotorPower = onLeft?1:-0.1;
	leftMotorPower *= onForward?1:-1;
	rightMotorPower *= onForward?1:-1;
	SetMotor(leftMotor, leftMotorPower);
	SetMotor(rightMotor,rightMotorPower);
	Wait(1.45);
	SetMotor(leftMotor, 0);
	SetMotor(rightMotor,0);
}

// white -> red -> yellow -> green -> light blue -> blue -> purple -> black
// const int __reportLight__[8][3] = {{1, 1, 1}, {1, 0, 0}, {1, 1, 0}, 
//                                    {0, 1, 0}, {0, 1, 1}, {0, 0, 1},
//                                    {1, 0, 1}, {0, 0, 0}};
const int __reportLight__[4][2] = {{0,0}, {1, 0}, {0, 1}, {1, 1}};
const int __n_reportLight__ = 4;
void reportZone(int zone) {
	// int r = __reportLight__[zone%__n_reportLight__][0];
	int g = __reportLight__[zone%__n_reportLight__][1];
	int b = __reportLight__[zone%__n_reportLight__][2];
	// SetPin(redLEDPin, r);s
	SetPin(blueLEDPin, g);
    SetPin(greenLEDPin, b);
}