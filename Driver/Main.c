#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include <RASLib/inc/servo.h>

static tServo *leftServo;
static tServo *rightServo;
static tBoolean initialized = false;
static tADC *leftIRSensor;
static tADC *rightIRSensor;
static wallDistance = 10;
void setup(void) {
    if (initialized) {
        return;
    }

    initialized = true;
    
    leftServo = InitializeServo(PIN_B1);
    rightServo  = InitializeServo(PIN_B2);
    leftIRSensor = InitializeADC(PIN_B3);
    rightIRSensor = InitializeADC(PIN_B4);
}


// The 'main' function is the entry point of the program
int main(void) {
    // Initialization code can go here
    setup();
    float leftPosition = 0;
    float rightPosition = 1;

    while (1) {
        if (ADCRead(leftIRSensor) < ADCRead(rightIRSensor)) {
            if (ADCRead(leftSensor) < wallDistance) {
                leftPosition += 0.01f;
            }
            else if (ADCRead(leftSensor) > wallDistance) {
                leftPosition -= 0.01f;
            }
        }
        else {
            if (ADCRead(rightSensor) < wallDistance) {
                rightPosition -= 0.01f;
            }
            else if (ADCREad(leftSensor) < wallDistance) {
                rightPosition += 0.01f;
            }
        }

        SetServo(leftServo,leftPosition);
        SetServo(rightServo,rightPosition);
    }
}
