#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include <RASLib/inc/motor.h>
#include <RASLib/inc/adc.h>
#include <RASLib/inc/linesensor.h>
#include <RASLib/inc/servo.h>
// #include </home/mixs/ras/Rasware/RASLib/inc/common.h>
// #include </home/mixs/ras/Rasware/RASLib/inc/gpio.h>
// #include </home/mixs/ras/Rasware/RASLib/inc/time.h>
// #include </home/mixs/ras/Rasware/RASLib/inc/motor.h>
// #include </home/mixs/ras/Rasware/RASLib/inc/adc.h>
// #include </home/mixs/ras/Rasware/RASLib/inc/linesensor.h>

//define components
tBoolean initialized = false;
tMotor *leftMotor;
tMotor *rightMotor;
tServo *gateServo;
tADC *leftSensor;
tADC *rightSensor;
tLineSensor *lineSensor;

//define constants
#define distanceOfTheWall 0.8 // TODO
#define leftMotorPin PIN_B6
#define rightMotorPin PIN_A4
#define gateServoPin PIN_B2
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
#define nLineSensor 8

//variables
tBoolean driveLeft;
tBoolean runPrimary;
tBoolean running;

void setup();
void chooseLeft();
void chooseRight();
void choosePrimary();
void chooseSecondary();
int getLine();
float getDistance(tADC *sensorPin);
tBoolean isLine();
tBoolean isBlack(float line);
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
	// white -> red -> yellow -> green -> light blue -> blue -> purple -> isBlack
    reportZone(0); // black
	setup();
	// walkForward(true);

    //use button to choose side
    // reportZone(1) // green
    // running = false;
    // while(!running){+
    // 	CallOnPinRising(chooseLeft, 0, PIN_F0);
    // 	CallOnPinRising(chooseRight,0, PIN_F4);
    // }

    //use button to choose stage
    reportZone(2) // blue
    running = true;
    while(!running) {
    	CallOnPinRising(choosePrimary, 0, PIN_F0);
    	CallOnPinRising(chooseSecondary,0, PIN_F4);
    }

    if (runPrimary) {
    	// zone1: escape the box
		reportZone(1); // green
		SetMotor(leftMotor, 1);
		SetMotor(rightMotor, 1);
		while(!isLineThick()) {
		}
		walkForward(true);
	
		// zone2: get the middle one
		reportZone(2); // blue
		while(!isLineSplit()) {
			walkLine();
		}
		walkForward(true);	//to make sure we can capture the pokemon in the center
		turn90Degree(true, driveLeft)
		walkForward(true);
	
		// zone3: find the wall
		reportZone(3); // light blue
		while(!isTheWall(!driveLeft)) {
			walkLine();
		}
		walkForward(true);
	
		// zone4: get the ball
		reportZone(4); // black
		grabSideBall(!driveLeft);
		walkForward(true);
	
		// zone5: capture and turn aronud
		reportZone(5); // green
		while(!isTheWall(!driveLeft)) {
			walkLine();
		}
		walkForward(true);
		turnAround();
		walkForward(true);
	
		// zone6: detect edge of our field
		reportZone(6); // blue
		while(!isLineSplit()) {
			walkLine();
		}
		walkForward(true);
		walkForward(true);
		walkForward(true);
	
		// zone7: crossing fields
		reportZone(7); // light blue
		SetMotor(leftMotor, 1);
		SetMotor(rightMotor, 1);
		while(!isLine()) {
		}
		turn90Degree(true, driveLeft);
		walkForward(true);
	
		// zone8: get'em!!!
		reportZone(8); // black
		while(true) { // TODO, what to detect the end?
			walkLine();
		}
	}
	else {
		driveLeft = !driveLeft; // switch since face different way
		// zone1: escape the box
		reportZone(1); // green
		SetMotor(leftMotor, 1);
		SetMotor(rightMotor, 1);
		while(!isLineThick()) {
		}
		walkForward(true);
	
		// zone2: find the wall
		reportZone(2); // blue
		while(!isTheWall(driveLeft)) {
			walkLine();
		}
		walkForward(true);
	
		// zone3: get the ball
		reportZone(3); // light blue
		grabSideBall(driveLeft);
		walkForward(true);
	
		// zone4: find intersection to the pokemon
		reportZone(4); // black
		while(!isLineSplit()) {
			walkLine();
		}
		turn90Degree(true, driveLeft);
		walkForward(true);
	
		// zone5: go and get it
		reportZone(5); // black
		while(true) { // TODO, what to detect the end?
			walkLine();
		}
	}
}

void setup(void) {
	if (initialized) {
		return;
	  }

	initialized = true;
	running = false;

	//driveLeft = true; choose by pressing button

	leftMotor = InitializeServoMotor(leftMotorPin, false);
	rightMotor  = InitializeServoMotor(rightMotorPin, true);
	gateServo = InitializeServo(gateServoPin);
	leftSensor = InitializeADC(leftSensorPin);
	rightSensor = InitializeADC(rightSensorPin);
	ADCReadContinuously(leftSensor,0.01);
	ADCReadContinuously(rightSensor,0.01);
	lineSensor = InitializeGPIOLineSensor(lineSensorList);

	float left = getDistance(leftSensor);
	float right = getDistance(rightSensor);
	if (left < right) {
		driveLeft = true;
	}
	else {
		driveLeft = false;
	}
}

//use button to choose side of field
void chooseLeft(void) {
	running = true;
	driveLeft = true;
}

void chooseRight(void) {
	running = true;
	driveLeft = false;
}

//use button to choose status of game
void choosePrimary(void) {
	running = true;
	runPrimary = true;
}

void chooseSecondary(void) {
	running = true;
	runPrimary = false;
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
        if (isBlack(line[i])) {
        	total += i;
        	count += 1;
        	lineFound = true;	        
        }
    }

    if (lineFound) {current = total/count;}
    Printf("%d\n",current);
    return current;
}

float getDistance(tADC *sensor) {
	// return 7.83/ADCRead(sensorPin)-1.66;
	// TODO: Average value for more accurate reading
	return ADCRead(sensor);
}

tBoolean isLine(void) {
	float line[8];
	int i, state = 0;

	LineSensorReadArray(lineSensor, line);

	for(i=0; i<nLineSensor; i++) {
		if(isBlack(line[i]))
			return true;
	}
	return false;
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

tBoolean isTheWall(tBoolean dL) {
	tADC *focusSensor;
	if(dL)
		focusSensor = leftSensor;
	else
		focusSensor = rightSensor;
	return getDistance(focusSensor) > distanceOfTheWall;
}

void walkLine(void) { // TODO
	tBoolean closed = false;
	int current = getLine();

    switch(current){
    	case 0:  //turn right
    		// blueLight();
    		// SetServo(gateServo,0.5);
    		// closed = true;
    		while (!(current == 3 || current == 4)) {
	    		SetMotor(leftMotor,0.5);
	    		SetMotor(rightMotor,-0.2);
	    		current = getLine();
	    	}
	    	//greenLight();
    		break;
    	case 1:
    		// SetServo(gateServo,0.5);
    		// closed = true;
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
    		// SetServo(gateServo,0.5);
    		// closed = true;
    		SetMotor(leftMotor,0);
    		SetMotor(rightMotor,0.5);
    		break;
    	case 7:  //turn left
    		// blueLight();
    		// SetServo(gateServo,0.5);
    		closed = true;
    		while (!(current == 3 || current == 4)) {
	    		SetMotor(leftMotor,-0.2);
	    		SetMotor(rightMotor,0.5);
	    		current = getLine();
	    	}
	    	// greenLight();
    		break;
    }
    if (closed) {
    	SetServo(gateServo,0);
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
	Wait(0.3);
	SetMotor(leftMotor, 0);
	SetMotor(rightMotor,0);
}

void grabBall() {
	SetServo(gateServo,0.5);
	Wait(1);
	SetServo(gateServo,0);
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
