#ifndef __DEVICE_H
#define __DEVICE_H

#define MAX_DEVICES 16

typedef struct {
    const char* serialNumber;
    const char* name;
    int   dhcp;
} device_t;


device_t *devices[MAX_DEVICES];


#endif