#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <zlog.h>
#include <string.h>
#include "devices.h"

//forward decl
void *logHandler;


char* getTypeString(int type){
    if (type == DIGITAL_INPUT)
        return "DIN";
    if (type == DIGITAL_OUTPUT)
        return "DOUT";
    if (type == ANALOG_INPUT)
        return "AIN";
    if (type == ANALOG_OUTPUT)
        return "AOUT";
    else
        return "UNKNOWN";
}

void dumpDevice(device_t *device){
    printf("\nDevice Name: %s (#%s)\n",device->name,device->serialNumber);
    printf("%9s %16.16s %9s %9s %9s \n", "index","name", "pin", "type", "default");
    printf("%9s %16.16s %9s %9s %9s \n", "-----","----", "---", "----", "-------");

    for (int y = 0; y < device->numberOfPins; y++){
        device_port_t *port = device->pins[y];
        printf("%9i %16.16s %9d %9s %9d \n", y,port->name, port->pin, getTypeString(port->type), port->defaultValue);
    }
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
