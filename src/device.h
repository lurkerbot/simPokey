#ifndef __DEVICE_H
#define __DEVICE_H

#define MAX_DEVICES 16

#define UKNOWN_PIN_TYPE -1
#define DIGITAL_INPUT 0
#define DIGITAL_OUTPUT 1

typedef struct{
    const char* name;
    int pin;
    int type;
    int defaultValue;
} device_port_t;

typedef struct {
    const char* serialNumber;
    const char* name;
    int   dhcp;
    device_port_t *ports[];
} device_t;

device_t *devices[MAX_DEVICES];


#endif