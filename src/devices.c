#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <zlog.h>
#include <string.h>
#include "devices.h"

//forward decl
void *logHandler;

void dumpDevice(device_t *device){
    printf("Device Name: %s (#%s)\n",device->name,device->serialNumber);
    printf("%9s %9s %9s %9s %9s \n", "index","name", "pin", "type", "default");

        for (int y = 0; y < device->numberOfPins; y++){
            device_port_t *port = device->pins[y];
            printf("%9i %9s %9d %9d %9d \n", y,port->name, port->pin, port->type, port->defaultValue);
        }
        printf("\n");
}

void dumpDevices() {
    zlog_info(logHandler, "Dumping device to stdout");

    for (int i = 0; i < activeDevices; i++){
        dumpDevice(devices[i]);

    }
}

int getDeviceBySerialNumber(device_t* device, char* serialNumber){
    for (int i = 0; i < activeDevices; i++)
    {
        device_t *searchDevice = devices[i];
        if (strcmp(serialNumber, searchDevice->serialNumber)==0){
            memcpy(device, searchDevice, sizeof(device_t));
            return 1;
        }
    }
    return 0;
}
