#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Type = (Manual), Alias = (Button::Radio))
        class CHICANE_GRID ButtonRadio : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Button::Radio";

            // Attributes
            static constexpr inline const char* GROUP_ATTRIBUTE_NAME    = "group";
            static constexpr inline const char* VALUE_ATTRIBUTE_NAME    = "value";
            static constexpr inline const char* CHECKED_ATTRIBUTE_NAME  = "checked";
            static constexpr inline const char* ON_INPUT_ATTRIBUTE_NAME = "onInput";

            // Values
            static constexpr inline const char* STATE_CHECKED = "checked";
            static constexpr inline const char* STATE_IDLE    = "idle";

        public:
            CH_CONSTRUCTOR()
            ButtonRadio(const pugi::xml_node& inNode);

        public:
            bool isFocusable() const override;
            bool onEvent(const WindowEvent& inEvent) override;

        protected:
            void onTick(float inDeltaTime) override;
            void onClick() override;

        public:
            CH_FUNCTION()
            void select();

        private:
            void setChecked(bool inValue, bool bShouldEmit);
            void refreshAttributes();
            void refreshState();
            void clearGroup();
            void clearGroupFrom(Component* inNode);
            void commit();
            void emitInput();
            bool parseChecked(const String& inValue, bool inFallback) const;

        public:
            CH_FIELD()
            bool isChecked;

            CH_FIELD()
            String group;

            CH_FIELD()
            String value;

            CH_FIELD()
            String checkState;

        private:
            bool m_bIsEdited;
        };
    }
}
