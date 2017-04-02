#ifndef __DEVICES_H
#define __DEVICES_H

#include "../../PoKeysLib.h"

#define MAX_DEVICES 16
#define MAX_PINS 55
#define MAX_DEVICE_NAME_LENGTH 10

typedef struct{
    const char* name;
    int pin;
    int type;
    int defaultValue;
    int valid;
} device_port_t;

typedef struct {
    const char* serialNumber;
    const char* name;
    int hasPokey;
    int dhcp;
    int numberOfPins;
    sPoKeysDevice* pokey;
    device_port_t *pins[MAX_PINS];
} device_t;

device_t *devices[MAX_DEVICES];
int activeDevices;

/* function prototypes */
void dumpDevices();
void dumpDevice(device_t *device);
int getDeviceBySerialNumber(device_t* device, char *serialNumber);
int syncDeviceName(device_t *device);


#endif
