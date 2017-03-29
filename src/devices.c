#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <zlog.h>
#include <string.h>
#include "devices.h"

//forward decl
void *logHandler;

char *getTypeString(int type)
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
        return "TRG_IN";
    if (pinFunction == PK_PinCap_digitalCounter)
        return "CNT";
    if (pinFunction == PK_PinCap_invertPin)
        return "INV";
    else
        return "??";
}

void dumpDevice(device_t *device)
{
    printf("\nDevice Name: %s (#%s)\n", device->name, device->serialNumber);

    if (device->hasPokey)
    {
      

        printf("Device Type: %s ", device->pokey->DeviceData.DeviceTypeName);
        printf("Firmware: v%d.%d.%d ", (device->pokey->DeviceData.FirmwareVersionMajor >> 4) + 1,
               device->pokey->DeviceData.FirmwareVersionMajor & 0x0F,
               device->pokey->DeviceData.FirmwareVersionMinor);

        printf("IP Address: %d.%d.%d.%d\n", (int)device->pokey->netDeviceData->IPAddressCurrent[0],
               (int)device->pokey->netDeviceData->IPAddressCurrent[1],
               (int)device->pokey->netDeviceData->IPAddressCurrent[2],
               (int)device->pokey->netDeviceData->IPAddressCurrent[3]);

        printf("Pins: %d ", device->pokey->info.iPinCount);
        printf("Analog: %d ", device->pokey->info.iAnalogInputs);
        printf("PWM: %d/%dMhz ", device->pokey->info.iPWMCount, (device->pokey->info.PWMinternalFrequency) / 1000000);
        printf("Encoders: %d (%db/%df/%duf)\n\n", device->pokey->info.iEncodersCount, device->pokey->info.iBasicEncoderCount, device->pokey->info.iFastEncoders, device->pokey->info.iUltraFastEncoders);

        PK_PinConfigurationGet(device->pokey);
        PK_EncoderConfigurationGet(device->pokey);
        printf("%9s  %9s %9s %9s \n", "pin",  "Use", "type", "default");
        printf("%9s %9s %9s %9s \n", "-----",  "---", "----", "-------");

        
        for(int i=0; i<device->pokey->info.iEncodersCount-1;i++){
            printf("%d\n",(int)device->pokey->Encoders[i].reserved[1]);
            printf("%d\n",device->pokey->Encoders[i].channelBpin);
        }

        for (int i = 0; i < device->pokey->info.iPinCount- 1; i++)
        {
                if (device->pokey->Pins[i].PinFunction==PK_PinCap_pinRestricted)
                    continue;

            printf("%9d %9s\n", i + 1,  getPinFunction(device->pokey->Pins[i].PinFunction));
        }

        printf("\n");
    }

    printf("%9s %16.16s %9s %9s %9s \n", "index", "name", "pin", "type", "default");
    printf("%9s %16.16s %9s %9s %9s \n", "-----", "----", "---", "----", "-------");

    for (int y = 0; y < device->numberOfPins ; y++)
    {
        device_port_t *port = device->pins[y];
        printf("%9i %16.16s %9d %9s %9d \n", y, port->name, port->pin, getTypeString(port->type), port->defaultValue);
    }
}

void dumpDevices()
{
    zlog_info(logHandler, "Dumping device to stdout");

    for (int i = 0; i < activeDevices; i++)
    {
        dumpDevice(devices[i]);
    }
}

int getDeviceBySerialNumber(device_t *device, char *serialNumber)
{
    for (int i = 0; i < activeDevices; i++)
    {
        device_t *searchDevice = devices[i];

        if (strcmp(serialNumber, searchDevice->serialNumber) == 0)
        {
            memcpy(device, searchDevice, sizeof(device_t));
            return 1;
        }
    }
    return 0;
}
