#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <zlog.h>
#include <string.h>
#include "../../encoder/encoder.h"
#include "../../pin/pin.h"
#include "pokey.h"


//forward decl
void *logHandler;

int syncDeviceName(device_t *device)
{
    if (strncmp((char *)device->pokey->DeviceData.DeviceName, device->name, MAX_DEVICE_NAME_LENGTH) != 0)
    {

        memcpy(device->pokey->DeviceData.DeviceName, device->name, MAX_DEVICE_NAME_LENGTH);
        int ret = PK_DeviceNameSet(device->pokey);

        if (ret != PK_OK)
        {
            printf("PK_DeviceNameSet: Err %d\n", ret);
            return -1;
        }

        zlog_info(logHandler, " - Reset device name %s", device->name);
    }

    return 1;
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
        printf("Encoders: %d (%db/%df/%duf)\n\n", device->pokey->info.iEncodersCount,
               device->pokey->info.iBasicEncoderCount,
               device->pokey->info.iFastEncoders,
               device->pokey->info.iUltraFastEncoders);

        // dumpEncoders(device->pokey);
        PK_PinConfigurationGet(device->pokey);

        printf("%9s  %9s %9s %9s \n", "pin", "Use", "type", "default");
        printf("%9s %9s %9s %9s \n", "-----", "---", "----", "-------");

        for (int i = 0; i < device->pokey->info.iPinCount - 1; i++)
        {
            if (device->pokey->Pins[i].PinFunction == PK_PinCap_pinRestricted)
                continue;

            printf("%9d %9s\n", i + 1, (char *)getPinFunction(device->pokey->Pins[i].PinFunction));
        }

        printf("\n");
    }

    printf("%9s %16.16s %9s %9s %9s \n", "index", "name", "pin", "type", "default");
    printf("%9s %16.16s %9s %9s %9s \n", "-----", "----", "---", "----", "-------");

    for (int y = 0; y < device->numberOfPins; y++)
    {
        device_port_t *port = device->pins[y];
        printf("%9i %16.16s %9d %9s %9d \n", y, port->name,
               port->pin,
               (char *)getPinTypeString(port->type),
               port->defaultValue);
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
