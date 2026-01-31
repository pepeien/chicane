#pragma once

#include <cstdint>

#include "Chicane/Screech.hpp"

namespace Chicane
{
    namespace Screech
    {
        // Extracted from `miniaudio/ma_sound_flags`
        enum class SoundFlag : std::uint16_t
        {
            /* Resource manager flags. */
            Stream        = 0x00000001, /* MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM */
            Decode        = 0x00000002, /* MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE */
            Async         = 0x00000004, /* MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC */
            WaitInit      = 0x00000008, /* MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_WAIT_INIT */
            UnknownLength = 0x00000010, /* MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_UNKNOWN_LENGTH
                                             */
            Looping       = 0x00000020, /* MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_LOOPING */

            /* ma_sound specific flags. */
            DefaultAttachment = 0x00001000, /* Do not attach to the endpoint by default.
                                               Useful for when setting up nodes in a
                                               complex graph system. */
            NoPitch           = 0x00002000, /* Disable pitch shifting with ma_sound_set_pitch()
                                               and ma_sound_group_set_pitch(). This is an
                                               optimization. */
            NoSpatialization  = 0x00004000  /* Disable spatialization. */
        };
    }
}