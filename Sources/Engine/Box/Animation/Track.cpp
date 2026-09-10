#include "Chicane/Box/Animation/Track.hpp"

namespace Chicane
{
    namespace Box
    {
        AnimationTrack::AnimationTrack()
            : name(""),
              keyframes({})
        {}

        AnimationTrack::AnimationTrack(const String& inName)
            : AnimationTrack()
        {
            name = inName;
        }

        void AnimationTrack::addKeyframe(const AnimationKeyframe& inValue)
        {
            keyframes.push_back(inValue);
        }
    }
}
