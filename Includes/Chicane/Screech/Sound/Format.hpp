#pragma once

#include "Chicane/Screech/Essential.hpp"

namespace Chicane
{
    namespace Screech
    {
        // Extracted from `miniaudio/ma_format`
        enum class SoundFormat : std::uint8_t
        {
            /*
            I like to keep these explicitly defined because they're used as a key into a lookup table. When items are
            added to this, make sure there are no gaps and that they're added to the lookup table in ma_get_bytes_per_sample().
            */
            Unknown = 0,     /* Mainly used for indicating an error, but also used as the default for the output format for decoders. */
            U8      = 1,
            S16     = 2,     /* Seems to be the most widely supported format. */
            S24     = 3,     /* Tightly packed. 3 bytes per sample. */
            S32     = 4,
            F32     = 5,
            Count
        };
    }
}