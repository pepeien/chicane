#include "ChannelPath.hpp"

namespace Chicane
{
    String toString(Box::ChannelPath inValue)
    {
        switch (inValue)
        {
        case Box::ChannelPath::Translation:
            return "Translation";

        case Box::ChannelPath::Rotation:
            return "Rotation";

        case Box::ChannelPath::Scale:
            return "Scale";

        default:
            return "";
        }
    }
}
