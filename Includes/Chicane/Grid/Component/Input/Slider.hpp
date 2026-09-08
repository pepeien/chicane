#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Type = (Manual), Alias = (Input::Slider))
        class CHICANE_GRID InputSlider : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Input::Slider";

            // Attributes
            static constexpr inline const char* VALUE_ATTRIBUTE_NAME    = "value";
            static constexpr inline const char* MIN_ATTRIBUTE_NAME      = "min";
            static constexpr inline const char* MAX_ATTRIBUTE_NAME      = "max";
            static constexpr inline const char* STEP_ATTRIBUTE_NAME     = "step";
            static constexpr inline const char* ON_INPUT_ATTRIBUTE_NAME = "onInput";

        public:
            CH_CONSTRUCTOR()
            InputSlider(const pugi::xml_node& inNode);

        public:
            bool isFocusable() const override;
            bool onEvent(const WindowEvent& inEvent) override;

        protected:
            void onTick(float inDeltaTime) override;

        public:
            CH_FUNCTION()
            void setValue(float inValue);

        private:
            void refreshRange();
            void applyAt(const Vec2& inLocation);
            void nudge(int inSteps);
            void commit();
            void emitInput();
            float parseNumber(const String& inValue, float inFallback) const;
            float clampValue(float inValue) const;
            float snapValue(float inValue) const;
            void refreshPercentage();

        public:
            CH_FIELD()
            float value;
            CH_FIELD()
            float min;
            CH_FIELD()
            float max;
            CH_FIELD()
            float step;
            CH_FIELD()
            float percentage;

        private:
            bool m_bIsSliding;
            bool m_bIsEdited;
        };
    }
}
