#ifndef __DEVICES_H
#define __DEVICES_H

#include "PoKeysLib.h"

#define MAX_DEVICES 16
#define MAX_PINS 55
#define MAX_DEVICE_NAME_LENGTH 10

#define UKNOWN_PIN_TYPE -1
#define DIGITAL_INPUT 0
#define DIGITAL_OUTPUT 1
#define ANALOG_INPUT 2
#define ANALOG_OUTPUT 3

typedef struct{
    const char* name;
    int pin;
    int type;
    int defaultValue;
} device_port_t;

typedef struct {
    const char* serialNumber;
    const char* name;
    int dhcp;
    int numberOfPins;
    sPoKeysDevice* pokey;
    device_port_t *pins[MAX_PINS];
} device_t;

device_t *devices[MAX_DEVICES];
int activeDevices;

void dumpDevices();
void dumpDevice(device_t *device);
int getDeviceBySerialNumber(device_t* device, char *serialNumber);

#endif