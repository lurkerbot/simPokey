#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <zlog.h>
#include <string.h>

#include "libProSimDataSource.h"

#include "config.h"
#include "../device/pokey/pokey.h"
#include "../pin/pin.h"

// forward decl
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

int loadConfiguredDevices()
{
    config_setting_t *configurationDevices = config_lookup(&configuration, "configuration.devices");
    int numberOfDevices = config_setting_length(configurationDevices);

    for (int i = 0; i < numberOfDevices; i++)
    {
        const char *name, *serialNumber;
        int dhcp, uid;

        device_t *device = malloc(sizeof(device_t));
        config_setting_t *configurationDevice = config_setting_get_elem(configurationDevices, i);

        config_setting_lookup_string(configurationDevice, "name", &name);
        config_setting_lookup_string(configurationDevice, "serialNumber", &serialNumber);
        config_setting_lookup_bool(configurationDevice, "dhcp", &dhcp);
        config_setting_lookup_int(configurationDevice, "uid", &uid);

        device->name = malloc(MAX_DEVICE_NAME_LENGTH);
        device->name = name;
        device->serialNumber = serialNumber;
        device->dhcp = dhcp;
        device->uid = uid;

        /* Set defaults */
        device->hasPokey = 0;

        zlog_info(logHandler, "%s (#%s)", device->name, device->serialNumber);
        loadPinConfiguration(config_setting_get_member(configurationDevice, "pins"), device);
        loadPWMConfiguration(config_setting_get_member(configurationDevice, "pwm"), device);

        devices[i] = device;
    }

    zlog_info(logHandler, "Completed loading %d devices", numberOfDevices);
    activeDevices = numberOfDevices;

    return 0;
}

int loadSimulatorDetails()
{
    char *simType;
    char *simIPAddress;
    int simPort;

    sim_set_logging_handler(logHandler);

    config_setting_t *simulator = config_lookup(&configuration, "configuration.simulator");
    if (simulator == NULL)
    {
        zlog_info(logHandler, "No simulator configuration found");
        return (EXIT_FAILURE);
    }

    int rv = config_setting_length(simulator);
    if (rv == 0)
    {
        zlog_info(logHandler, " - Invalid simulator config found");
        return (EXIT_FAILURE);
    }

    zlog_info(logHandler, "Loading simulator configuration");
    config_setting_t *simulatorConfig = config_setting_get_elem(simulator, 0);

    config_setting_lookup_string(simulatorConfig, "type", &simType);
    if (simType == NULL)
    {
        zlog_info(logHandler, " - No simualator type specified");
        return (EXIT_FAILURE);
    }
    zlog_info(logHandler, " - %s set as simulator type", simType);
   
   
    config_setting_lookup_string(simulatorConfig, "ipAddress", &simIPAddress);
    if (simIPAddress == NULL)
    {
        zlog_info(logHandler, " - No simualator IP Address specified");
        return (EXIT_FAILURE);
    }
   
    config_setting_lookup_int(simulatorConfig, "port", &simPort);
    if (simPort == NULL)
    {
        zlog_info(logHandler, " - No simualator port specified");
        return (EXIT_FAILURE);
    }
    zlog_info(logHandler, " - Simulator configured %s:%d", simIPAddress,simPort);

    simConfig = malloc(sizeof(simulator_t));
    
    simConfig->type = malloc(64);
    simConfig->type = simType;
    simConfig->ipAddress = malloc(16);
    simConfig->ipAddress = simIPAddress;
    simConfig->port = simPort;

    
}
