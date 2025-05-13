#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE Text : public Component
        {
        public:
            // Tag
            static constexpr const char* TAG_ID = "Text";

        public:
            Text(const pugi::xml_node& inNode)
                : Component(inNode)
            {}
        };
    }
}