#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Type = (Manual), Alias = (Input::ButtonSlider))
        class CHICANE_GRID InputButtonSlider : public Container
        {
        public:
            static constexpr inline const char* TAG_ID = "Input::ButtonSlider";

            static constexpr inline const char* VALUE_ATTRIBUTE_NAME     = "value";
            static constexpr inline const char* MIN_ATTRIBUTE_NAME       = "min";
            static constexpr inline const char* MAX_ATTRIBUTE_NAME       = "max";
            static constexpr inline const char* STEP_ATTRIBUTE_NAME      = "step";
            static constexpr inline const char* LABEL_ATTRIBUTE_NAME     = "label";
            static constexpr inline const char* PRECISION_ATTRIBUTE_NAME = "precision";
            static constexpr inline const char* ON_INPUT_ATTRIBUTE_NAME  = "onInput";

        public:
            CH_CONSTRUCTOR()
            InputButtonSlider(const XmlNode& inNode);

        public:
            bool isFocusable() const override;
            bool onEvent(const WindowEvent& inEvent) override;

        protected:
            void onTick(float inDeltaTime) override;

        public:
            CH_FUNCTION()
            void setValue(float inValue);

            CH_FUNCTION()
            void decrement();

            CH_FUNCTION()
            void increment();

            CH_FUNCTION()
            void commitText();

        private:
            void refreshRange();
            void refreshLabel();
            void applyAt(const Vec2& inLocation);
            void nudge(int inSteps, float inScale = 1.0f);
            void commit();
            void emitInput();
            void refreshPercentage();
            void refreshValueText();
            bool isEditing() const;

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
            CH_FIELD()
            int precision;
            CH_FIELD()
            String label;
            CH_FIELD()
            String valueText;

        private:
            bool m_bIsSliding;
            bool m_bIsEdited;
        };
    }
}
