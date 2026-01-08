#pragma once

#include "Chicane/Screech.hpp"
#include "Chicane/Screech/Sound/Channel.hpp"
#include "Chicane/Screech/Sound/Format.hpp"

namespace Chicane
{
    namespace Screech
    {
        // Extracted from `miniaudio/ma_device_info/nativeDataFormats`
        struct CHICANE_SCREECH DeviceFormat
        {
        public:
            DeviceFormat();

        public:
            SoundFormat   format;     /* Sample format. If set to ma_format_unknown, all sample formats are supported. */
            std::uint32_t channels;   /* If set to 0, all channels are supported. */
            std::uint32_t sampleRate; /* If set to 0, all sample rates are supported. */
            std::uint32_t flags;      /* A combination of MA_DATA_FORMAT_FLAG_* flags. */
        };
    }
}