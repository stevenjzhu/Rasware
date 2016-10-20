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
static int wallDistance;

float leftPosition;
float rightPosition;

void setup(void) {
    if (initialized) {
        return;
    }

    initialized = true;

    wallDistance = 10;
    leftPosition = 0;
    rightPosition = 1;
    leftServo = InitializeServo(PIN_B1);
    rightServo  = InitializeServo(PIN_B2);
    leftSensor = InitializeADC(PIN_B3);
    rightSensor = InitializeADC(PIN_B4);
}

void wallAdjust(void) {
  if (ADCRead(leftSensor) < ADCRead(rightSensor)) {
      if (ADCRead(leftSensor) < wallDistance) {
          leftPosition += 0.01f;
      }
      else if (ADCRead(leftSensor) > wallDistance) {
          leftPosition -= 0.01f;
      }
  }
  else if (ADCRead(leftSensor) > ADCRead(rightSensor)) {
      if (ADCRead(rightSensor) < wallDistance) {
          rightPosition -= 0.01f;
      }
      else if (ADCRead(leftSensor) < wallDistance) {
          rightPosition += 0.01f;
      }
  }
}
// The 'main' function is the entry point of the program
int main(void) {
    // Initialization code can go here
    setup();

    while (1) {
        wallAdjust();

        // Set servo speed
        SetServo(leftServo,leftPosition);
        SetServo(rightServo,rightPosition);
    }
}
