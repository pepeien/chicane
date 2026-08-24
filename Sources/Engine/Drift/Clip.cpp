#include "Chicane/Drift/Clip.hpp"

namespace Chicane
{
    namespace Drift
    {
        Clip::Clip()
            : name(""),
              duration(0.0f),
              loop(Loop::Once),
              iterations(1),
              tracks({})
        {}

        Clip::Clip(const String& inName)
            : Clip()
        {
            name = inName;
        }

        Track* Clip::getTrack(const String& inName)
        {
            for (Track& track : tracks)
            {
                if (track.name.equals(inName))
                {
                    return &track;
                }
            }

            return nullptr;
        }

        const Track* Clip::getTrack(const String& inName) const
        {
            for (const Track& track : tracks)
            {
                if (track.name.equals(inName))
                {
                    return &track;
                }
            }

            return nullptr;
        }

        void Clip::addTrack(const Track& inValue)
        {
            if (Track* existing = getTrack(inValue.name))
            {
                *existing = inValue;

                return;
            }

            tracks.push_back(inValue);
        }
    }
}
