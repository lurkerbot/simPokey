#include <stdio.h>
#include <zlog.h>
#include <string.h>
#include <stdlib.h>

#include "../PoKeysLib.h"
#include "../config/config.h"
#include "pin.h"

//forward decl
void *logHandler;

char *getPinTypeString(int type)
{
    if (type == DIGITAL_INPUT)
        return "DIN";
    if (type == DIGITAL_OUTPUT)
        return "DOUT";
    if (type == ANALOG_INPUT)
        return "AIN";
    if (type == ENCODER)
        return "ENC";
    if (type == FAST_ENCODER)
        return "FENC";
    if (type == UFAST_ENCODER)
        return "UFENC";
    if (type == PWM)
        return "PWM";
    if (type == TRIGGER)
        return "TRIGGER";
    if (type == COUNTER)
        return "COUNTER";
    else
        return "UNKNOWN";
}

char *getPinFunction(int pinFunction)
{
    if (pinFunction == PK_PinCap_pinRestricted)
        return "Not_Used";
    if (pinFunction == PK_PinCap_reserved)
        return "Reserved";
    if (pinFunction == PK_PinCap_digitalInput)
        return "DIN";
    if (pinFunction == PK_PinCap_digitalOutput)
        return "DOUT";
    if (pinFunction == PK_PinCap_analogInput)
        return "AIN";
    if (pinFunction == PK_PinCap_analogOutput)
        return "AOUT";
    if (pinFunction == PK_PinCap_triggeredInput)
        return "TRG";
    if (pinFunction == PK_PinCap_digitalCounter)
        return "CNT";
    if (pinFunction == PK_PinCap_invertPin)
        return "INV";
    else
        return "??";
}

int checkValidPinConfiguration(device_t *device, int pin)
{
    if (!device->hasPokey)
    {
        return -1;
    }

    PK_PinConfigurationGet(device->pokey);

    for (int i = 0; i < device->numberOfPins; i++)
    {
        device_port_t *pin = device->pins[i];

        char *str = NULL;
        int strLen = asprintf(&str, "- Error: %s (%s) on pin %d", pin->name, getPinTypeString(pin->type), pin->pin);

        switch (pin->type)
        {
        case DIGITAL_INPUT:
            if (PK_CheckPinCapability(device->pokey, pin->pin, PK_AllPinCap_digitalInput) == 0)
                strcat(str, " - Incorrect type DIGITAL_INPUT");
            else
                pin->valid = 1;
            break;
        case DIGITAL_OUTPUT:
            if (PK_CheckPinCapability(device->pokey, pin->pin, PK_AllPinCap_digitalOutput) == 0)
                strcat(str, " - Incorrect type DIGITAL_OUTPUT");
            else
                pin->valid = 1;
            break;
        case ENCODER:
            if (PK_CheckPinCapability(device->pokey, pin->pin, PK_AllPinCap_digitalInput) == 0)
                strcat(str, " - Incorrect type ENCODER");
            else
                pin->valid = 1;
            break;
        case FAST_ENCODER:
            if (pin->pin != 1 & pin->pin != 2 & pin->pin != 5 & pin->pin != 6 & pin->pin != 15 & pin->pin != 16)
                strcat(str, " - Incorrect type FAST_ENCODER");
            else
                pin->valid = 1;
            break;
        case UFAST_ENCODER:
            if (pin->pin != 8 & pin->pin != 12 & pin->pin != 13)
                strcat(str, " - Incorrect type UFAST_ENCODER");
            else
                pin->valid = 1;
            break;
        case ANALOG_INPUT:
            if (PK_CheckPinCapability(device->pokey, pin->pin, PK_AllPinCap_analogInput) == 0 &
                PK_CheckPinCapability(device->pokey, pin->pin, PK_AllPinCap_MFanalogInput) == 0)
                strcat(str, " - Incorrect type ANALOG_INPUT");
            else
                pin->valid = 1;
            break;
        case PWM:
            if (PK_CheckPinCapability(device->pokey, pin->pin, PK_AllPinCap_PWMOut) == 0)
                strcat(str, " - Incorrect type PWM");
            else
                pin->valid = 1;
            break;
        case COUNTER:
            if (PK_CheckPinCapability(device->pokey, pin->pin, PK_AllPinCap_digitalCounter) == 0)
                strcat(str, " - Incorrect type COUNTER");
            else
                pin->valid = 1;
            break;
        case TRIGGER:
            strcat(str, " - Incorrect type TRIGGER");
            break;
        }

        if (!pin->valid)
            zlog_info(logHandler, "%s", str);
    }
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

int loadPinConfiguration(config_setting_t *configuredPorts, device_t *device)
{
    int pin, defaultValue = 0, type = 0, ret, pinIndex = 0, numberOfPins = 0;
    const char *name, *tempType;

    if (configuredPorts == NULL)
    {
        zlog_info(logHandler, " - No pins section configuration");
        return NO_PINS;
    }

    numberOfPins = config_setting_length(configuredPorts);

    if (numberOfPins == 0)
    {
        zlog_info(logHandler, " - No pins configuration");
        return NO_PINS;
    }

    device_port_t *port = malloc(sizeof(device_port_t));
    memset(port, 0, sizeof(device_port_t));

    zlog_info(logHandler, " - Loading pins configuration");

    for (int i = 0; i < numberOfPins; i++)
    {
        int pin, defaultValue = 0, type = 0, ret;
        const char *name, *tempType;

        config_setting_t *configurationPort = config_setting_get_elem(configuredPorts, i);
        device_port_t *port = malloc(sizeof(device_port_t));
        memset(port, 0, sizeof(device_port_t));

        ret = config_setting_lookup_int(configurationPort, "pin", &pin);

        if (ret == CONFIG_FALSE)
        {
            zlog_info(logHandler, "No pin configuration. Skipping...");
            continue;
        }

        if (checkPinExistsInConfig(device, pin) == PIN_EXISTS)
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
        else if (strcmp("ANALOG_OUTPUT", (char *)tempType) == 0)
            type = ANALOG_OUTPUT;
        else
            type = UKNOWN_PIN_TYPE;

        port->name = name;
        port->pin = pin;
        port->valid = 0;
        port->defaultValue = defaultValue;
        port->type = type;
        port->value = 0;
        port->previousValue = 0;

        device->pins[pinIndex++] = port;
        device->numberOfPins++;
    }

    zlog_info(logHandler, " - Loaded %d/%d ports", device->numberOfPins, numberOfPins);

    return 0;
}

int applyPinConfigurationToDevice(device_t *device)
{
    for (int i = 0; i < device->numberOfPins; i++)
    {
        device_port_t *pin = device->pins[i];

        if (pin->valid == PIN_INVALID)
        {
            printf("Invalid pin %d...skipping\n", i);
            device->pokey->Pins[pin->pin - 1].PinFunction = PK_PinCap_reserved;
            continue;
        }

        if (pin->type == DIGITAL_OUTPUT)
        {
            device->pokey->Pins[pin->pin - 1].PinFunction = PK_PinCap_digitalOutput;
            device->pokey->Pins[pin->pin - 1].DigitalValueSet = pin->defaultValue;
            continue;
        }

        if (pin->type == DIGITAL_INPUT)
        {
            device->pokey->Pins[pin->pin - 1].PinFunction = PK_PinCap_digitalInput;
            continue;
        }
    }

PK_DigitalIOSet(device->pokey);
    int ret = PK_PinConfigurationSet(device->pokey);
    return ret;
}
