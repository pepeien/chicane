#include "Chicane/Box/Animation/Clip.hpp"

namespace Chicane
{
    namespace Box
    {
        AnimationClip::AnimationClip()
            : name(""),
              duration(0.0f),
              loop(AnimationLoop::Once),
              iterations(1),
              tracks({})
        {}

        AnimationClip::AnimationClip(const String& inName)
            : AnimationClip()
        {
            name = inName;
        }

        AnimationTrack* AnimationClip::getTrack(const String& inName)
        {
            for (AnimationTrack& track : tracks)
            {
                if (track.name.equals(inName))
                {
                    return &track;
                }
            }

            return nullptr;
        }

        const AnimationTrack* AnimationClip::getTrack(const String& inName) const
        {
            for (const AnimationTrack& track : tracks)
            {
                if (track.name.equals(inName))
                {
                    return &track;
                }
            }

            return nullptr;
        }

        void AnimationClip::addTrack(const AnimationTrack& inValue)
        {
            if (AnimationTrack* existing = getTrack(inValue.name))
            {
                *existing = inValue;

                return;
            }

            tracks.push_back(inValue);
        }
    }
}
