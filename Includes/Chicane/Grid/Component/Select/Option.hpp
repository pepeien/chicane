#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Button.hpp"

namespace Chicane
{
    namespace Grid
    {
        class Select;

        CH_TYPE(Manual)
        class CHICANE_GRID SelectOption : public Button
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Option";

            // Properties
            static constexpr inline const char* VALUE_ATTRIBUTE_NAME = "value";

        public:
            CH_CONSTRUCTOR()
            SelectOption(const pugi::xml_node& inNode);

        protected:
            void onRefresh() override;
            void onClick() override;

        public:
            String getLabel() const;

        private:
            void ensureText(const String& inText);
            void refreshValue();
            void refreshClass();
            Select* findSelect() const;

        public:
            CH_FIELD()
            String value;
        };
    }
}
