#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Animation/Loop.hpp"
#include "Chicane/Box/Animation/Track.hpp"

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX AnimationClip
        {
        public:
            AnimationClip();
            AnimationClip(const String& inName);

        public:
            AnimationTrack* getTrack(const String& inName);
            const AnimationTrack* getTrack(const String& inName) const;
            void addTrack(const AnimationTrack& inValue);

        public:
            String               name;
            float                duration;
            AnimationLoop        loop;
            int                  iterations;
            AnimationTrack::List tracks;
        };
    }
}
