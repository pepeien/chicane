#include "Chicane/Screech/Device/Id.hpp"

#include <miniaudio.h>

namespace Chicane
{
    namespace Screech
    {
        DeviceId::DeviceId(void* inData)
            : DeviceId()
        {
            const ma_device_id* data = (ma_device_id*)(inData);

            std::copy(std::begin(data->dsound), std::end(data->dsound), dsound.begin());

            wasapi    = data->wasapi;
            winmm     = data->winmm;
            alsa      = data->alsa;
            pulse     = data->pulse;
            jack      = data->jack;
            coreaudio = data->coreaudio;
            sndio     = data->sndio;
            audio4    = data->audio4;
            oss       = data->oss;
            aaudio    = data->aaudio;
            opensl    = data->opensl;
            webaudio  = data->webaudio;
        }

        DeviceId::DeviceId()
            : wasapi(""),
            dsound({}),
            winmm(0),
            alsa(""),
            pulse(""),
            jack(0),
            coreaudio(""),
            sndio(""),
            audio4(""),
            oss(""),
            aaudio(0),
            opensl(0),
            webaudio("")
        {}
    }
}