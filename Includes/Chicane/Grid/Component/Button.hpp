#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Button : public Component
        {
        public:
            // Header
            static constexpr const char* TAG_ID = "Button";

            // Attributes
            static constexpr const char* ON_CLICK_ATTRIBUTE_NAME = "onClick";
            static constexpr const char* ON_CLICK_EVENT_KEYWORD  = "$event";

        public:
            Button(const pugi::xml_node& inNode);

        public:
            virtual void onClick(const Event& inEvent) { return; }
        };
    }
}