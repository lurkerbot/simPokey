#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <zlog.h>
#include <string.h>
#include "config.h"
#include "../device/pokey/pokey.h"
#include "../pin/pin.h"

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

    zlog_info(logHandler, "Loaded config: %s (v%s)", fileName, getConfigurationValue("version"));
    zlog_info(logHandler, "Loaded config: %s", getConfigurationValue("name"));

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

int checkPinExistsInConfig(device_t *device, int pin)
{
    for (int i = 0; i < device->numberOfPins; i++)
    {
        if (pin == device->pins[i]->pin)
        {
            return PIN_EXISTS;
        }
    }
    return PIN_FREE;
}

int loadPorts(config_setting_t *configurationDevice, device_t *device)
{
    int numberOfPorts;
    config_setting_t *devicePorts = config_setting_get_member(configurationDevice, "ports");

    if (devicePorts == NULL)
    {
        zlog_info(logHandler, " - No ports detected");
        return 0;
    }

    numberOfPorts = config_setting_length(devicePorts);
    int pinIndex = 0;

    for (int i = 0; i < numberOfPorts; i++)
    {

        int pin, defaultValue = 0, type = 0;
        int ret;
        const char *name, *tempType;

        config_setting_t *configurationPort = config_setting_get_elem(devicePorts, i);
        device_port_t *port = malloc(sizeof(device_port_t));
        memset(port, 0, sizeof(device_port_t));

        ret = config_setting_lookup_int(configurationPort, "pin", &pin);

        if (ret == CONFIG_FALSE)
        {
            zlog_info(logHandler, "No pin configuration. Skipping...");
            continue;
        }

        if (checkPinExistsInConfig(device, pin)==PIN_EXISTS)
        {
            zlog_info(logHandler, "  - Duplicate pin %d. Skipping", pin);
            continue;
        }
    

        ret = config_setting_lookup_string(configurationPort, "type", &tempType);
        if (ret == CONFIG_FALSE)
        {
            zlog_info(logHandler, "No pin direction specified. Skipping...");
            continue;
        }

        ret = config_setting_lookup_string(configurationPort, "name", &name);
        if (ret == CONFIG_FALSE)
        {
            name = "Undefined";
        }

        config_setting_lookup_int(configurationPort, "default", &defaultValue);

        if (strcmp("DIGITAL_INPUT", (char *)tempType) == 0)
            type = DIGITAL_INPUT;
        else if (strcmp("DIGITAL_OUTPUT", (char *)tempType) == 0)
            type = DIGITAL_OUTPUT;
        else if (strcmp("ANALOG_INPUT", (char *)tempType) == 0)
            type = ANALOG_INPUT;
        else if (strcmp("ENCODER", (char *)tempType) == 0)
            type = ENCODER;
        else if (strcmp("FAST_ENCODER", (char *)tempType) == 0)
            type = FAST_ENCODER;
        else if (strcmp("UFAST_ENCODER", (char *)tempType) == 0)
            type = UFAST_ENCODER;
        else if (strcmp("PWM", (char *)tempType) == 0)
            type = PWM;
        else if (strcmp("TRIGGER", (char *)tempType) == 0)
            type = TRIGGER;
        else if (strcmp("COUNTER", (char *)tempType) == 0)
            type = COUNTER;
        else
            type = UKNOWN_PIN_TYPE;

        port->name = name;
        port->pin = pin;
        port->valid = 0;
        port->defaultValue = defaultValue;
        port->type = type;

        device->pins[pinIndex++] = port;
        device->numberOfPins++;
                
    }

    zlog_info(logHandler, " - Loaded %d/%d ports", device->numberOfPins,numberOfPorts);

    return 0;
}

int loadConfiguredDevices()
{
    config_setting_t *configurationDevices = config_lookup(&configuration, "configuration.devices");
    int numberOfDevices = config_setting_length(configurationDevices);

    for (int i = 0; i < numberOfDevices; i++)
    {
        const char *name, *serialNumber;
        int dhcp;

        device_t *device = malloc(sizeof(device_t));
        config_setting_t *configurationDevice = config_setting_get_elem(configurationDevices, i);

        config_setting_lookup_string(configurationDevice, "name", &name);
        config_setting_lookup_string(configurationDevice, "serialNumber", &serialNumber);
        config_setting_lookup_bool(configurationDevice, "dhcp", &dhcp);

        device->name = malloc(MAX_DEVICE_NAME_LENGTH);
        device->name = name;
        device->serialNumber = serialNumber;
        device->dhcp = dhcp;

        /* Set defaults */
        device->hasPokey = 0;

        zlog_info(logHandler, "%s (#%s)", device->name, device->serialNumber);
        loadPorts(configurationDevice, device);
        devices[i] = device;
    }

    zlog_info(logHandler, "Completed loading %d devices", numberOfDevices);
    activeDevices = numberOfDevices;

    return 0;
}
