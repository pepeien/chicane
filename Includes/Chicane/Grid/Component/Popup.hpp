#pragma once

#include "Core.hpp"
#include "Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE Popup : public Component
        {
        public:
            // Tag
            static constexpr const char* TAG_ID = "Popup";

            // Attributes
            static constexpr const char* IS_OPEN_ATTRIBUTE_NAME        = "isOpen";
            static constexpr const char* IS_CONSTRAINED_ATTRIBUTE_NAME = "isConstrained";
            static constexpr const char* IS_RESIZABLE_ATTRIBUTE_NAME   = "isResizable";
        };
    }
}