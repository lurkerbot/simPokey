#ifndef __PIN_H
#define __PIN_H

#include "../PoKeysLib.h"
#include "../devices.h"

#define UKNOWN_PIN_TYPE 0
#define DIGITAL_INPUT 1
#define DIGITAL_OUTPUT 2
#define ANALOG_INPUT 3
#define ENCODER 4
#define FAST_ENCODER 5
#define UFAST_ENCODER 6
#define PWM 7
#define COUNTER 8
#define TRIGGER 9       //NOT CURRENTLY SUPPORTED

char *getPinTypeString(int type);
char *getPinFunction(int pinFunction);
int checkValidPinConfiguration(device_t* device, int pin);

#endif
