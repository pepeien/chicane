#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Text.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID TextInput : public Text
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "TextInput";

            // Attributes
            static constexpr inline const char* VALUE_ATTRIBUTE_NAME    = "value";
            static constexpr inline const char* ON_INPUT_ATTRIBUTE_NAME = "onInput";

        public:
            CH_CONSTRUCTOR()
            TextInput(const pugi::xml_node& inNode);

        public:
            bool isFocusable() const override;

            bool onEvent(const WindowEvent& inEvent) override;

        protected:
            void onRefresh() override;
            void onFocus() override;
            void onBlur() override;

            void refreshSize() override;

        private:
            void refreshValue();
            void insert(const String& inText);
            void erase();
            void commit();
            void emitInput();
            void setTextInputActive(bool inValue);

        public:
            CH_FIELD()
            String value;

            CH_FIELD()
            float caretX;

        private:
            bool   m_bEdited;
            String m_pendingText;
        };
    }
}
