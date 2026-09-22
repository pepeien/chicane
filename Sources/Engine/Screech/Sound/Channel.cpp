#include "Chicane/Screech/Sound/Channel.hpp"

namespace Chicane
{
    String toString(Screech::SoundChannel inValue)
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
