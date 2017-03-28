#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <zlog.h>
#include <string.h>
#include "config.h"
#include "devices.h"

//forward decl
void *logHandler;

int configIsLoaded = 0;

int initConfiguration(config_t *config, const char *fileName)
{
    config_init(config);
    if (!config_read_file(config, fileName))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(config),
                config_error_line(config), config_error_text(config));
        config_destroy(config);
        return (EXIT_FAILURE);
    }

    configIsLoaded = 1;

    zlog_info(logHandler,"Loaded config: %s (v%s)", fileName, getConfigurationValue("version"));
    zlog_info(logHandler,"Loaded config: %s", getConfigurationValue("name"));

    return (CONFIG_TRUE);
}

void *getConfigurationValue(char *element)
{
    const char *str;
    if (config_lookup_string(&configuration, element, &str))
        return (char *)str;
    else
        zlog_info(logHandler, "No %s setting in configuration file", element);

    return 0;
}


int loadPorts(config_setting_t *configurationDevice, device_t *device){
    int numberOfPorts;
    config_setting_t *devicePorts = config_setting_get_member(configurationDevice, "ports");

    if (devicePorts == NULL)
    {
        zlog_info(logHandler, " - No ports detected");
        return 0;
    }
   
    numberOfPorts = config_setting_length(devicePorts);

    for (int i = 0; i < numberOfPorts; i++){

        int pin, defaultValue=0,type=0;
        int ret;
        const char *name, *tempType;

        device_port_t *port = malloc(sizeof(device_port_t));
        memset(port, 0, sizeof(device_port_t));

        config_setting_t *configurationPort = config_setting_get_elem(devicePorts, i);

        ret = config_setting_lookup_int(configurationPort, "pin", &pin);

        if (ret == CONFIG_FALSE){
            zlog_info(logHandler, "No pin configuration. Skipping...");
            continue;
        }

        ret = config_setting_lookup_string(configurationPort, "type", &tempType);
        if (ret == CONFIG_FALSE){
            zlog_info(logHandler, "No pin direction specified. Skipping...");
            continue;
        }

        ret = config_setting_lookup_string(configurationPort, "name", &name);
         if (ret == CONFIG_FALSE){
             name = "Undefined";
         }

         config_setting_lookup_int(configurationPort, "default", &defaultValue);
         
         if (strcmp("DIGITAL_INPUT", (char *)tempType) == 0)
             type = DIGITAL_INPUT;
        else if (strcmp("DIGITAL_OUTPUT",(char*)tempType)==0)
            type = DIGITAL_OUTPUT;
        else 
            type = -1;
    
        port->name = name;
        port->pin = pin;
        port->defaultValue = defaultValue;
        port->type = type;

        device->pins[i] = port;
        device->numberOfPins++;

        //zlog_info(logHandler, "%s,%d,%d,%d (name,pin,default,type)", port->name, port->pin, port->defaultValue, port->type);
    }

    zlog_info(logHandler, " - Loaded %d ports", numberOfPorts);

    return 0;
}

int loadConfiguredDevices() {
    config_setting_t* configurationDevices = config_lookup(&configuration, "configuration.devices");
    int numberOfDevices = config_setting_length(configurationDevices);

    
    for (int i = 0; i < numberOfDevices; i++){
        const char *name, *serialNumber;
        int dhcp;

        device_t *device = malloc(sizeof(device_t));
        config_setting_t *configurationDevice = config_setting_get_elem(configurationDevices, i);

        config_setting_lookup_string(configurationDevice, "name", &name);
        config_setting_lookup_string(configurationDevice, "serialNumber", &serialNumber);
        config_setting_lookup_bool(configurationDevice, "dhcp", &dhcp);

        device->name = name;
        device->serialNumber = serialNumber;
        device->dhcp = dhcp;

        zlog_info(logHandler,"%s (#%s)", device->name, device->serialNumber);
        loadPorts(configurationDevice,device);
        devices[i] = device;
    }

        zlog_info(logHandler,"Completed loading %d devices", numberOfDevices);
        activeDevices = numberOfDevices;

        return 0;
}

