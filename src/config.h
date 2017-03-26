#ifndef __CONFIG_H
#define __CONFIG_H

typedef struct {
    const char* serialNumber;
    const char* name;

} configuration_device_item;

config_t configuration;


int initConfiguration(config_t *config, const char *fileName);
void* getConfigurationValue(char* element);
int getConfigurationList(config_setting_t* item, char* element);

#endif