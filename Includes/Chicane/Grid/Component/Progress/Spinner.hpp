#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Type = (Manual), Alias = (Progress::Spinner))
        class CHICANE_GRID ProgressSpinner : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Progress::Spinner";

            // Attributes
            static constexpr inline const char* IS_SPINNING_ATTRIBUTE_NAME = "bIsSpinning";

        public:
            CH_CONSTRUCTOR()
            ProgressSpinner(const XmlNode& inNode);

        public:
            CH_FIELD()
            bool bIsSpinning;
        };
    }
}
