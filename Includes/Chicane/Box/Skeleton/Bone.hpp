#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX SkeletonBone
        {
        public:
            using List = std::vector<SkeletonBone>;

        public:
            static constexpr inline const char* TAG                        = "Bone";
            static constexpr inline const char* ID_ATTRIBUTE_NAME          = "id";
            static constexpr inline const char* TRANSLATION_ATTRIBUTE_NAME = "translation";
            static constexpr inline const char* ROTATION_ATTRIBUTE_NAME    = "rotation";
            static constexpr inline const char* SCALE_ATTRIBUTE_NAME       = "scale";

        public:
            SkeletonBone();

            void setFrom(const pugi::xml_node& inNode);
            void saveTo(pugi::xml_node& outNode) const;

        public:
            String    id;
            Transform transform;
            List      children;
        };

        struct CHICANE_BOX SkeletonBoneEntry
        {
        public:
            using List = std::vector<SkeletonBoneEntry>;

        public:
            SkeletonBoneEntry();

        public:
            String       id;
            std::int32_t parentIndex;
            Transform    transform;
            Mat4         inverseBind;
        };
    }
}
