#pragma once

#include <cstdint>

#include "Chicane/Screech.hpp"
#include "Chicane/Core/String.hpp"

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
            Custom, /* <-- Custom backend, with callbacks defined by the context
                       config. */
            Null    /* <-- Must always be the last item. Lowest priority, and used
                       as the terminator for backend enumeration. */
        };
    }

    inline String toString(Screech::SoundVendor inValue)
    {
        switch (inValue)
        {
        case Screech::SoundVendor::WASAPI:
            return "WASAPI";

        case Screech::SoundVendor::DirectSound:
            return "DirectSound";

        case Screech::SoundVendor::WinMM:
            return "WinMM";

        case Screech::SoundVendor::CoreAudio:
            return "CoreAudio";

        case Screech::SoundVendor::Sndio:
            return "Sndio";

        case Screech::SoundVendor::Audio4:
            return "Audio4";

        case Screech::SoundVendor::OSS:
            return "OSS";

        case Screech::SoundVendor::PulseAudio:
            return "PulseAudio";

        case Screech::SoundVendor::ALSA:
            return "ALSA";

        case Screech::SoundVendor::JACK:
            return "JACK";

        case Screech::SoundVendor::AAudio:
            return "AAudio";

        case Screech::SoundVendor::OpenSL:
            return "OpenSL";

        case Screech::SoundVendor::WebAudio:
            return "WebAudio";

        case Screech::SoundVendor::Custom:
            return "Custom";

        case Screech::SoundVendor::Null:
            return "Null";

        default:
            return "";
        }
    }
}
