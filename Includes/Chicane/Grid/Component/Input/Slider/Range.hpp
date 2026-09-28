#pragma once

#include <cstdint>

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
        enum class InputRangeSliderDrag : std::uint8_t
        {
            None,
            Low,
            High,
            Span
        };

        CH_TYPE(Type = (Manual), Alias = (Input::RangeSlider))
        class CHICANE_GRID InputRangeSlider : public Container
        {
        public:
            static constexpr inline const char* TAG_ID = "Input::RangeSlider";

            static constexpr inline const char* LOW_ATTRIBUTE_NAME       = "low";
            static constexpr inline const char* HIGH_ATTRIBUTE_NAME      = "high";
            static constexpr inline const char* MIN_ATTRIBUTE_NAME       = "min";
            static constexpr inline const char* MAX_ATTRIBUTE_NAME       = "max";
            static constexpr inline const char* STEP_ATTRIBUTE_NAME      = "step";
            static constexpr inline const char* PRECISION_ATTRIBUTE_NAME = "precision";
            static constexpr inline const char* ON_INPUT_ATTRIBUTE_NAME  = "onInput";

        public:
            CH_CONSTRUCTOR()
            InputRangeSlider(const XmlNode& inNode);

        public:
            bool onEvent(const WindowEvent& inEvent) override;

        protected:
            void onTick(float inDeltaTime) override;

        public:
            CH_FUNCTION()
            void setLow(float inValue);

            CH_FUNCTION()
            void setHigh(float inValue);

            CH_FUNCTION()
            void commitLowText();

            CH_FUNCTION()
            void commitHighText();

        private:
            void refreshRange();
            void applyAt(const Vec2& inLocation);
            void applyRange(float inLow, float inHigh, InputRangeSliderDrag inMode);
            void nudge(int inSteps, float inScale = 1.0f);
            void commit();
            void emitInput();
            void refreshPercents();
            void refreshValueText();
            InputRangeSliderDrag hitDrag(const Vec2& inLocation) const;
            bool isEditing() const;
            bool isEditingLow() const;
            bool isEditingHigh() const;
            float gap() const;

        public:
            CH_FIELD()
            float low;

            CH_FIELD()
            float high;

            CH_FIELD()
            float min;

            CH_FIELD()
            float max;

            CH_FIELD()
            float step;

            CH_FIELD()
            float lowPercent;

            CH_FIELD()
            float spanPercent;

            CH_FIELD()
            int precision;

            CH_FIELD()
            String lowText;

            CH_FIELD()
            String highText;

        private:
            InputRangeSliderDrag m_drag;
            float                m_spanWidth;
            float                m_grabOffset;
            bool                 m_bIsEdited;
        };
    }
}
