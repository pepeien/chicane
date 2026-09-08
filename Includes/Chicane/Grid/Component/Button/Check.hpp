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
        CH_TYPE(Type = (Manual), Alias = (Button::Check))
        class CHICANE_GRID ButtonCheck : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Button::Check";

            // Attributes
            static constexpr inline const char* CHECKED_ATTRIBUTE_NAME  = "checked";
            static constexpr inline const char* ON_INPUT_ATTRIBUTE_NAME = "onInput";

            // Values
            static constexpr inline const char* STATE_CHECKED = "checked";
            static constexpr inline const char* STATE_IDLE    = "idle";

        public:
            CH_CONSTRUCTOR()
            ButtonCheck(const pugi::xml_node& inNode);

        public:
            bool isFocusable() const override;
            bool onEvent(const WindowEvent& inEvent) override;

        protected:
            void onTick(float inDeltaTime) override;
            void onClick() override;

        public:
            CH_FUNCTION()
            void toggle();

            CH_FUNCTION()
            void check();

            CH_FUNCTION()
            void uncheck();

        private:
            void setChecked(bool inValue);
            void refreshChecked();
            void refreshState();
            void commit();
            void emitInput();
            bool parseChecked(const String& inValue, bool inFallback) const;

        public:
            CH_FIELD()
            bool isChecked;

            CH_FIELD()
            String checkState;

        private:
            bool m_bIsEdited;
        };
    }
}
