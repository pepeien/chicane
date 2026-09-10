#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Animation/Clip.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Reference.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Animation : public Asset
        {
        public:
            static constexpr inline const char* EXTENSION = ".bani";
            static constexpr inline const char* TAG       = "Animation";

            static constexpr inline const char* CLIP_TAG                       = "Clip";
            static constexpr inline const char* CLIP_DURATION_ATTRIBUTE_NAME   = "duration";
            static constexpr inline const char* CLIP_LOOP_ATTRIBUTE_NAME       = "loop";
            static constexpr inline const char* CLIP_ITERATIONS_ATTRIBUTE_NAME = "iterations";

            static constexpr inline const char* TRACK_TAG                 = "Track";
            static constexpr inline const char* TRACK_NAME_ATTRIBUTE_NAME = "name";

            static constexpr inline const char* KEYFRAME_TAG                   = "Keyframe";
            static constexpr inline const char* KEYFRAME_TIME_ATTRIBUTE_NAME   = "time";
            static constexpr inline const char* KEYFRAME_EASING_ATTRIBUTE_NAME = "easing";
            static constexpr inline const char* TRANSLATION_ATTRIBUTE_NAME     = "translation";
            static constexpr inline const char* ROTATION_ATTRIBUTE_NAME        = "rotation";
            static constexpr inline const char* SCALE_ATTRIBUTE_NAME           = "scale";

        public:
            Animation(const FileSystem::Path& inFilepath);
            virtual ~Animation() = default;

        public:
            bool hasSkeleton() const;
            const AssetReference& getSkeleton() const;
            void setSkeleton(const FileSystem::Path& inSource);
            void setSkeleton(const AssetReference& inValue);

            const AnimationClip& getClip() const;
            void setClip(const AnimationClip& inClip);

        private:
            void fetchSkeleton();
            void fetchClip();
            void writeSkeleton();
            void writeClip();

        private:
            AssetReference m_skeleton;
            AnimationClip  m_clip;
        };
    }
}
