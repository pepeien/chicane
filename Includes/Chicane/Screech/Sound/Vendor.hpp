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

        inline String toString(SoundVendor inValue)
        {
            switch (inValue)
            {
            case SoundVendor::WASAPI:
                return "WASAPI";

            case SoundVendor::DirectSound:
                return "DirectSound";

            case SoundVendor::WinMM:
                return "WinMM";

            case SoundVendor::CoreAudio:
                return "CoreAudio";

            case SoundVendor::Sndio:
                return "Sndio";

            case SoundVendor::Audio4:
                return "Audio4";

            case SoundVendor::OSS:
                return "OSS";

            case SoundVendor::PulseAudio:
                return "PulseAudio";

            case SoundVendor::ALSA:
                return "ALSA";

            case SoundVendor::JACK:
                return "JACK";

            case SoundVendor::AAudio:
                return "AAudio";

            case SoundVendor::OpenSL:
                return "OpenSL";

            case SoundVendor::WebAudio:
                return "WebAudio";

            case SoundVendor::Custom:
                return "Custom";

            case SoundVendor::Null:
                return "Null";

            default:
                return "";
            }
        }
    }
}