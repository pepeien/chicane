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
            // File
            static constexpr inline const char* EXTENSION = ".bani";

            // Tag
            static constexpr inline const char* TAG = "Animation";

            // Attributes
            static constexpr inline const char* TRANSLATION_ATTRIBUTE_NAME = "translation";
            static constexpr inline const char* ROTATION_ATTRIBUTE_NAME    = "rotation";
            static constexpr inline const char* SCALE_ATTRIBUTE_NAME       = "scale";

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
