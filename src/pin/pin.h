#ifndef __PIN_H
#define __PIN_H

#ifdef __cplusplus
extern “C” {
#endif

#include "../PoKeysLib.h"
#include "../device/pokey/pokey.h"

#define PIN_FREE        0
#define PIN_EXISTS      1

#define PIN_INVALID     0
#define PIN_VALID       1

#define NO_PINS         1

#define UKNOWN_PIN_TYPE 0
#define DIGITAL_INPUT 1
#define DIGITAL_OUTPUT 2
#define ANALOG_INPUT 3
#define ANALOG_OUTPUT 4
#define ENCODER 5
#define FAST_ENCODER 6
#define UFAST_ENCODER 7
#define PWM_INPUT 8
#define COUNTER 9
#define TRIGGER 10

char *getPinTypeString(int type);
char *getPinFunction(int pinFunction);
int checkValidPinConfiguration(device_t* device, int pin);
int loadPinConfiguration(config_setting_t* configuredPorts,device_t *device);
int loadPWMConfiguration(config_setting_t *configuredPorts, device_t *device);
int checkPinExistsInConfig(device_t *device, int pin);
int applyPinConfigurationToDevice(device_t *device);

#ifdef __cplusplus
}
#endif

#endif
