#pragma once

#include "Chicane/Core/String.hpp"
#include "Chicane/Screech.hpp"
#include "Chicane/Screech/Sound/Channel.hpp"
#include "Chicane/Screech/Sound/Format.hpp"

namespace Chicane
{
    namespace Screech
    {
        // Extracted from `miniaudio/ma_device_id`
        struct CHICANE_SCREECH DeviceId
        {
        public:
            DeviceId(void* inData);
            DeviceId();

        public:
            String                       wasapi;    /* WASAPI uses a wchar_t string for identification. */
            std::array<std::uint8_t, 16> dsound;    /* DirectSound uses a GUID for identification. */
            std::uint32_t                winmm;     /* When creating a device, WinMM expects a Win32 UINT_PTR for device identification. In practice it's actually just a UINT. */
            String                       alsa;      /* ALSA uses a name string for identification. */
            String                       pulse;     /* PulseAudio uses a name string for identification. */
            int                          jack;      /* JACK always uses default devices. */
            String                       coreaudio; /* Core Audio uses a string for identification. */
            String                       sndio;     /* "snd/0", etc. */
            String                       audio4;    /* "/dev/audio", etc. */
            String                       oss;       /* "dev/dsp0", etc. "dev/dsp" for the default device. */
            std::int32_t                 aaudio;    /* AAudio uses a 32-bit integer for identification. */
            std::uint32_t                opensl;    /* OpenSL|ES uses a 32-bit unsigned integer for identification. */
            String                       webaudio;  /* Web Audio always uses default devices for now, but if this changes it'll be a GUID. */
        };
    }
}