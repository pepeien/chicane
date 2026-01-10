#include "Chicane/Screech/Device/Format.hpp"

namespace Chicane
{
    namespace Screech
    {
        DeviceFormat::DeviceFormat()
            : format(SoundFormat::Unknown),
              channels(0),
              sampleRate(0),
              flags(0)
        {}
    }
}