#pragma once

#include "Chicane/Screech/Essential.hpp"

namespace Chicane
{
    namespace Screech
    {
        // Extracted from `miniaudio/ma_backend`
        enum class SoundVendor : std::uint8_t
        {
            WASAPI,
            DirectSound,
            WinMM,
            CoreAudio,
            Sndio,
            Audio4,
            OSS,
            PulseAudio,
            ALSA,
            JACK,
            AAudio,
            OpenSL,
            WebAudio,
            Custom,  /* <-- Custom backend, with callbacks defined by the context config. */
            Null     /* <-- Must always be the last item. Lowest priority, and used as the terminator for backend enumeration. */
        };
    }
}