#pragma once

#include "Chicane/Screech/Essential.hpp"

namespace Chicane
{
    namespace Screech
    {
        // Extracted from `miniaudio/_ma_channel_position`
        enum class SoundChannel : std::uint8_t
        {
            None               = 0,
            Mono               = 1,
            FrontLeft          = 2,
            FrontRight         = 3,
            FrontCenter        = 4,
            LFE                = 5,
            BackLeft           = 6,
            BackRight          = 7,
            FrontLeftCenter    = 8,
            FrontRightCenter   = 9,
            BackCenter         = 10,
            SideLeft           = 11,
            SideRight          = 12,
            TopCenter          = 13,
            TopFrontLeft       = 14,
            TopFrontCenter     = 15,
            TopFrontRight      = 16,
            TopBackLeft        = 17,
            TopBackCenter      = 18,
            TopBackRIght       = 19,
            Aux0               = 20,
            Aux1               = 21,
            Aux2               = 22,
            Aux3               = 23,
            Aux4               = 24,
            Aux5               = 25,
            Aux6               = 26,
            Aux7               = 27,
            Aux8               = 28,
            Aux9               = 29,
            Aux10              = 30,
            Aux11              = 31,
            Aux12              = 32,
            Aux13              = 33,
            Aux14              = 34,
            Aux15              = 35,
            Aux16              = 36,
            Aux17              = 37,
            Aux18              = 38,
            Aux19              = 39,
            Aux20              = 40,
            Aux21              = 41,
            Aux22              = 42,
            Aux23              = 43,
            Aux24              = 44,
            Aux25              = 45,
            Aux26              = 46,
            Aux27              = 47,
            Aux28              = 48,
            Aux29              = 49,
            Aux30              = 50,
            Aux31              = 51,
            Left               = FrontLeft,
            Right              = FrontRight,
            Count              = (Aux31 + 1)
        };
    }
}