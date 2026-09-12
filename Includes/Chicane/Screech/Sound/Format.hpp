#pragma once

#include <cstdint>

#include "Chicane/Screech.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Screech
    {
        // Extracted from `miniaudio/ma_format`
        enum class SoundFormat : std::uint8_t
        {
            /*
            I like to keep these explicitly defined because they're used as a
            key into a lookup table. When items are added to this, make sure
            there are no gaps and that they're added to the lookup table in
            ma_get_bytes_per_sample().
            */
            Unknown = 0, /* Mainly used for indicating an error, but also used as the
                            default for the output format for decoders. */
            U8      = 1,
            S16     = 2, /* Seems to be the most widely supported format. */
            S24     = 3, /* Tightly packed. 3 bytes per sample. */
            S32     = 4,
            F32     = 5,
            Count
        };

        inline String toString(SoundFormat inValue)
        {
            switch (inValue)
            {
            case SoundFormat::Unknown:
                return "Unknown";

            case SoundFormat::U8:
                return "U8";

            case SoundFormat::S16:
                return "S16";

            case SoundFormat::S24:
                return "S24";

            case SoundFormat::S32:
                return "S32";

            case SoundFormat::F32:
                return "F32";

            case SoundFormat::Count:
                return "Count";

            default:
                return "";
            }
        }
    }
}