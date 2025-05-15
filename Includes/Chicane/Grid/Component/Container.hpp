#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Container : public Component
        {
        public:
            // Tag
            static constexpr const char* TAG_ID = "Container";

        public:
            Container(const pugi::xml_node& inNode);

        public:
            std::vector<Vertex> getPrimitive() const override;
        };
    }
}