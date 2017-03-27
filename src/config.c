#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <string.h>
#include "config.h"


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

    printf("Loaded config: %s (v%s)\n", fileName, getConfigurationValue("version"));
    printf("Loaded config: %s\n", getConfigurationValue("name"));

    return (CONFIG_TRUE);
}

void *getConfigurationValue(char *element)
{
    const char *str;
    if (config_lookup_string(&configuration, element, &str))
        return (char *)str;
    else
        fprintf(stderr, "No %s setting in configuration file.\n", element);

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

        devices[i] = device;
        printf("%s - %s - %s \n", devices[i]->name, devices[i]->serialNumber,devices[i]->dhcp ? "DHCP":"Fixed");
    }

        printf("Loaded %d devices\n", numberOfDevices);


    return 0;
}
