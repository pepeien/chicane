#pragma once

#include <vector>

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Event.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Type = (Manual), Alias = (Input::Color))
        class CHICANE_GRID InputColor : public Container
        {
        public:
            static constexpr inline const char* TAG_ID = "Input::Color";

            static constexpr inline const char* VALUE_ATTRIBUTE_NAME    = "value";
            static constexpr inline const char* ON_INPUT_ATTRIBUTE_NAME = "onInput";

        public:
            CH_CONSTRUCTOR()
            InputColor(const XmlNode& inNode);

        public:
            bool isFocusable() const override;
            bool escapesOverflow() const override;
            bool onEvent(const WindowEvent& inEvent) override;

        protected:
            void onTick(float inDeltaTime) override;

        public:
            CH_FUNCTION()
            void toggle();

            CH_FUNCTION()
            void open();

            CH_FUNCTION()
            void close();

            CH_FUNCTION()
            void commitHex();

            CH_FUNCTION()
            void commitHsl();

            CH_FUNCTION()
            void commitHsv();

            CH_FUNCTION()
            void commitRgb();

            CH_FUNCTION()
            void commitModel();

            CH_FUNCTION()
            void setRgbModel();

            CH_FUNCTION()
            void setRgbaModel();

            CH_FUNCTION()
            void setHsvModel();

            CH_FUNCTION()
            void pickPreset(String inHex);

            CH_FUNCTION()
            void addPreset();

        private:
            bool isEditing() const;
            void refreshFromBinding();
            void applyToBinding();
            void emitInput();
            void applyRgb(float inR, float inG, float inB, float inA);
            void syncFromRgb(float inR, float inG, float inB, float inA);
            void refreshDerivedColors();
            void refreshModelFlags();
            void refreshWheel();
            void refreshTabState();
            Color::Rgba currentRgba() const;
            Component* findChildId(const String& inId) const;
            bool pickWheelAt(const Vec2& inLocation);
            bool pickValueAt(const Vec2& inLocation);

        public:
            CH_FIELD()
            String hex;

            CH_FIELD()
            String swatchHex;

            CH_FIELD()
            String vividHex;

            CH_FIELD()
            String grayHex;

            CH_FIELD()
            String toneHex;

            CH_FIELD()
            float hue;

            CH_FIELD()
            float saturation;

            CH_FIELD()
            float lightness;

            CH_FIELD()
            float brightness;

            CH_FIELD()
            float alpha;

            CH_FIELD()
            float red;

            CH_FIELD()
            float green;

            CH_FIELD()
            float blue;

            CH_FIELD()
            String model;

            CH_FIELD()
            std::vector<String> models;

            CH_FIELD()
            bool isHsl;

            CH_FIELD()
            bool isHsv;

            CH_FIELD()
            bool isRgb;

            CH_FIELD()
            bool isRgba;

            CH_FIELD()
            bool isOpen;

            CH_FIELD()
            String openState;

            CH_FIELD()
            String rgbState;

            CH_FIELD()
            String rgbaState;

            CH_FIELD()
            String hsvState;

            CH_FIELD()
            float wheelCursorX;

            CH_FIELD()
            float wheelCursorY;

            CH_FIELD()
            float valueDim;

            CH_FIELD()
            float valueThumbY;

            CH_FIELD()
            float alphaUnit;

            CH_FIELD()
            std::vector<String> presets;

        private:
            bool m_bIsPickingWheel;
            bool m_bIsPickingValue;
        };
    }
}
