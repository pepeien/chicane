#pragma once

#include <cstdint>

#include "Chicane/Screech.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Screech
    {
        // Extracted from `miniaudio/_ma_channel_position`
        enum class SoundChannel : std::uint8_t
        {
            None             = 0,
            Mono             = 1,
            FrontLeft        = 2,
            FrontRight       = 3,
            FrontCenter      = 4,
            LFE              = 5,
            BackLeft         = 6,
            BackRight        = 7,
            FrontLeftCenter  = 8,
            FrontRightCenter = 9,
            BackCenter       = 10,
            SideLeft         = 11,
            SideRight        = 12,
            TopCenter        = 13,
            TopFrontLeft     = 14,
            TopFrontCenter   = 15,
            TopFrontRight    = 16,
            TopBackLeft      = 17,
            TopBackCenter    = 18,
            TopBackRIght     = 19,
            Aux0             = 20,
            Aux1             = 21,
            Aux2             = 22,
            Aux3             = 23,
            Aux4             = 24,
            Aux5             = 25,
            Aux6             = 26,
            Aux7             = 27,
            Aux8             = 28,
            Aux9             = 29,
            Aux10            = 30,
            Aux11            = 31,
            Aux12            = 32,
            Aux13            = 33,
            Aux14            = 34,
            Aux15            = 35,
            Aux16            = 36,
            Aux17            = 37,
            Aux18            = 38,
            Aux19            = 39,
            Aux20            = 40,
            Aux21            = 41,
            Aux22            = 42,
            Aux23            = 43,
            Aux24            = 44,
            Aux25            = 45,
            Aux26            = 46,
            Aux27            = 47,
            Aux28            = 48,
            Aux29            = 49,
            Aux30            = 50,
            Aux31            = 51,
            Left             = FrontLeft,
            Right            = FrontRight,
            Count            = (Aux31 + 1)
        };

        inline String toString(SoundChannel inValue)
        {
            switch (inValue)
            {
            case SoundChannel::None:
                return "None";

            case SoundChannel::Mono:
                return "Mono";

            case SoundChannel::FrontLeft:
                return "FrontLeft";

            case SoundChannel::FrontRight:
                return "FrontRight";

            case SoundChannel::FrontCenter:
                return "FrontCenter";

            case SoundChannel::LFE:
                return "LFE";

            case SoundChannel::BackLeft:
                return "BackLeft";

            case SoundChannel::BackRight:
                return "BackRight";

            case SoundChannel::FrontLeftCenter:
                return "FrontLeftCenter";

            case SoundChannel::FrontRightCenter:
                return "FrontRightCenter";

            case SoundChannel::BackCenter:
                return "BackCenter";

            case SoundChannel::SideLeft:
                return "SideLeft";

            case SoundChannel::SideRight:
                return "SideRight";

            case SoundChannel::TopCenter:
                return "TopCenter";

            case SoundChannel::TopFrontLeft:
                return "TopFrontLeft";

            case SoundChannel::TopFrontCenter:
                return "TopFrontCenter";

            case SoundChannel::TopFrontRight:
                return "TopFrontRight";

            case SoundChannel::TopBackLeft:
                return "TopBackLeft";

            case SoundChannel::TopBackCenter:
                return "TopBackCenter";

            case SoundChannel::TopBackRIght:
                return "TopBackRIght";

            case SoundChannel::Aux0:
                return "Aux0";

            case SoundChannel::Aux1:
                return "Aux1";

            case SoundChannel::Aux2:
                return "Aux2";

            case SoundChannel::Aux3:
                return "Aux3";

            case SoundChannel::Aux4:
                return "Aux4";

            case SoundChannel::Aux5:
                return "Aux5";

            case SoundChannel::Aux6:
                return "Aux6";

            case SoundChannel::Aux7:
                return "Aux7";

            case SoundChannel::Aux8:
                return "Aux8";

            case SoundChannel::Aux9:
                return "Aux9";

            case SoundChannel::Aux10:
                return "Aux10";

            case SoundChannel::Aux11:
                return "Aux11";

            case SoundChannel::Aux12:
                return "Aux12";

            case SoundChannel::Aux13:
                return "Aux13";

            case SoundChannel::Aux14:
                return "Aux14";

            case SoundChannel::Aux15:
                return "Aux15";

            case SoundChannel::Aux16:
                return "Aux16";

            case SoundChannel::Aux17:
                return "Aux17";

            case SoundChannel::Aux18:
                return "Aux18";

            case SoundChannel::Aux19:
                return "Aux19";

            case SoundChannel::Aux20:
                return "Aux20";

            case SoundChannel::Aux21:
                return "Aux21";

            case SoundChannel::Aux22:
                return "Aux22";

            case SoundChannel::Aux23:
                return "Aux23";

            case SoundChannel::Aux24:
                return "Aux24";

            case SoundChannel::Aux25:
                return "Aux25";

            case SoundChannel::Aux26:
                return "Aux26";

            case SoundChannel::Aux27:
                return "Aux27";

            case SoundChannel::Aux28:
                return "Aux28";

            case SoundChannel::Aux29:
                return "Aux29";

            case SoundChannel::Aux30:
                return "Aux30";

            case SoundChannel::Aux31:
                return "Aux31";

            case SoundChannel::Count:
                return "Count";

            default:
                return "";
            }
        }
    }
}