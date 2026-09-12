#pragma once

#include <vector>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Animation/Keyframe.hpp"

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX AnimationTrack
        {
        public:
            using List = std::vector<AnimationTrack>;

        public:
            // Tag
            static constexpr inline const char* TAG = "Track";

            // Attributes
            static constexpr inline const char* NAME_ATTRIBUTE_NAME = "name";

        public:
            AnimationTrack();
            AnimationTrack(const String& inName);

        public:
            void addKeyframe(const AnimationKeyframe& inValue);

        public:
            String                  name;
            AnimationKeyframe::List keyframes;
        };
    }
}
