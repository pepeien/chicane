#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE Container : public Component
        {
        public:
            // Tag
            static constexpr const char* TAG_ID = "Container";

        public:
            Container(const pugi::xml_node& inNode)
                : Component(inNode)
            {}
        };
    }
}