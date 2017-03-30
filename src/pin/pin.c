#include <stdio.h>
#include <zlog.h>
#include <string.h>
#include <stdlib.h>

#include "../PoKeysLib.h"
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

    PK_PWMConfigurationGet(device->pokey);
    PK_EncoderConfigurationGet(device->pokey);
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
