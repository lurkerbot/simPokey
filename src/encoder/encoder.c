#include "encoder.h"

void dumpEncoders(sPoKeysDevice *pokey)
{

    if (pokey == NULL)
        return;

    int ret = PK_EncoderConfigurationGet(pokey);

    if (ret == PK_ERR_NOT_CONNECTED)
    {
        printf("dumpEncoders - not connected\n");
        return;
    }
    if (ret == PK_ERR_NOT_SUPPORTED)
    {
        printf("dumpEncoders - not supported\n");
        return;
    }

    printf("%5s %9s %9s %9s %5s %5s\n", "Encoder", "ChnlA", "ChnlB", "Enabled" ,"4x","2x");
    printf("%5s %9s %9s %9s %5s %5s\n", "-------", "-----", "-----", "-------", "--","--");

    for (int i = 0; i < pokey->info.iBasicEncoderCount; i++)
    {
        printf("%5d %9d %9d %5d %10d %5d\n", i + 1, pokey->Encoders[i].channelApin + 1,
               pokey->Encoders[i].channelBpin + 1,
               pokey->Encoders[i].encoderOptions&0b00000001,
               (pokey->Encoders[i].encoderOptions&0b00000010 ? 1 : 0),
               (pokey->Encoders[i].encoderOptions&0b00000100) ? 1 : 0);
    }
}
