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
    }

    inline String toString(Screech::SoundChannel inValue)
    {
        switch (inValue)
        {
        case Screech::SoundChannel::None:
            return "None";

        case Screech::SoundChannel::Mono:
            return "Mono";

        case Screech::SoundChannel::FrontLeft:
            return "FrontLeft";

        case Screech::SoundChannel::FrontRight:
            return "FrontRight";

        case Screech::SoundChannel::FrontCenter:
            return "FrontCenter";

        case Screech::SoundChannel::LFE:
            return "LFE";

        case Screech::SoundChannel::BackLeft:
            return "BackLeft";

        case Screech::SoundChannel::BackRight:
            return "BackRight";

        case Screech::SoundChannel::FrontLeftCenter:
            return "FrontLeftCenter";

        case Screech::SoundChannel::FrontRightCenter:
            return "FrontRightCenter";

        case Screech::SoundChannel::BackCenter:
            return "BackCenter";

        case Screech::SoundChannel::SideLeft:
            return "SideLeft";

        case Screech::SoundChannel::SideRight:
            return "SideRight";

        case Screech::SoundChannel::TopCenter:
            return "TopCenter";

        case Screech::SoundChannel::TopFrontLeft:
            return "TopFrontLeft";

        case Screech::SoundChannel::TopFrontCenter:
            return "TopFrontCenter";

        case Screech::SoundChannel::TopFrontRight:
            return "TopFrontRight";

        case Screech::SoundChannel::TopBackLeft:
            return "TopBackLeft";

        case Screech::SoundChannel::TopBackCenter:
            return "TopBackCenter";

        case Screech::SoundChannel::TopBackRIght:
            return "TopBackRIght";

        case Screech::SoundChannel::Aux0:
            return "Aux0";

        case Screech::SoundChannel::Aux1:
            return "Aux1";

        case Screech::SoundChannel::Aux2:
            return "Aux2";

        case Screech::SoundChannel::Aux3:
            return "Aux3";

        case Screech::SoundChannel::Aux4:
            return "Aux4";

        case Screech::SoundChannel::Aux5:
            return "Aux5";

        case Screech::SoundChannel::Aux6:
            return "Aux6";

        case Screech::SoundChannel::Aux7:
            return "Aux7";

        case Screech::SoundChannel::Aux8:
            return "Aux8";

        case Screech::SoundChannel::Aux9:
            return "Aux9";

        case Screech::SoundChannel::Aux10:
            return "Aux10";

        case Screech::SoundChannel::Aux11:
            return "Aux11";

        case Screech::SoundChannel::Aux12:
            return "Aux12";

        case Screech::SoundChannel::Aux13:
            return "Aux13";

        case Screech::SoundChannel::Aux14:
            return "Aux14";

        case Screech::SoundChannel::Aux15:
            return "Aux15";

        case Screech::SoundChannel::Aux16:
            return "Aux16";

        case Screech::SoundChannel::Aux17:
            return "Aux17";

        case Screech::SoundChannel::Aux18:
            return "Aux18";

        case Screech::SoundChannel::Aux19:
            return "Aux19";

        case Screech::SoundChannel::Aux20:
            return "Aux20";

        case Screech::SoundChannel::Aux21:
            return "Aux21";

        case Screech::SoundChannel::Aux22:
            return "Aux22";

        case Screech::SoundChannel::Aux23:
            return "Aux23";

        case Screech::SoundChannel::Aux24:
            return "Aux24";

        case Screech::SoundChannel::Aux25:
            return "Aux25";

        case Screech::SoundChannel::Aux26:
            return "Aux26";

        case Screech::SoundChannel::Aux27:
            return "Aux27";

        case Screech::SoundChannel::Aux28:
            return "Aux28";

        case Screech::SoundChannel::Aux29:
            return "Aux29";

        case Screech::SoundChannel::Aux30:
            return "Aux30";

        case Screech::SoundChannel::Aux31:
            return "Aux31";

        case Screech::SoundChannel::Count:
            return "Count";

        default:
            return "";
        }
    }
}
