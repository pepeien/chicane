#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Skeleton/Bone.hpp"
#include "Chicane/Box/Skeleton/Bone/Entry.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Skeleton : public Asset
        {
        public:
            static constexpr inline const char* EXTENSION = ".bskl";
            static constexpr inline const char* TAG       = "Skeleton";

        public:
            Skeleton(const FileSystem::Path& inFilepath);
            virtual ~Skeleton() = default;

        public:
            const SkeletonBone::List& getBones() const;
            void setBones(const SkeletonBone::List& inBones);

            const SkeletonBoneEntry::List& getEntries() const;
            const SkeletonBone* getBone(const String& inId) const;
            std::int32_t findIndex(const String& inId) const;
            bool hasBone(const String& inId) const;

        private:
            void fetchBones();
            void rebuildEntries();
            void writeBones();

        private:
            SkeletonBone::List      m_bones;
            SkeletonBoneEntry::List m_entries;
        };
    }
}
