#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Animation/Loop.hpp"
#include "Chicane/Box/Animation/Track.hpp"

#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Time.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX AnimationClip
        {
        public:
            // Tag
            static constexpr inline const char* TAG = "Clip";

            // Attributes
            static constexpr inline const char* DURATION_ATTRIBUTE_NAME   = "duration";
            static constexpr inline const char* LOOP_ATTRIBUTE_NAME       = "loop";
            static constexpr inline const char* ITERATIONS_ATTRIBUTE_NAME = "iterations";

        public:
            AnimationClip();
            AnimationClip(const String& inName);

        public:
            AnimationTrack* getTrack(const String& inName);
            const AnimationTrack* getTrack(const String& inName) const;
            void addTrack(const AnimationTrack& inValue);

        public:
            String               name;
            Time                 duration;
            AnimationLoop        loop;
            int                  iterations;
            AnimationTrack::List tracks;
        };
    }
}
