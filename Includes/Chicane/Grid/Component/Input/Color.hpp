#pragma once

#include <vector>

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
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
            InputColor(const pugi::xml_node& inNode);

        public:
            bool isFocusable() const override;

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
            void refreshLabels();
            Color::Rgba currentRgba() const;

        public:
            CH_FIELD()
            String hex;

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
            String hueText;

            CH_FIELD()
            String saturationText;

            CH_FIELD()
            String lightnessText;

            CH_FIELD()
            String brightnessText;

            CH_FIELD()
            String alphaText;

            CH_FIELD()
            String redText;

            CH_FIELD()
            String greenText;

            CH_FIELD()
            String blueText;

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
            bool isOpen;

            CH_FIELD()
            String openState;

            CH_FIELD()
            std::vector<String> presets;
        };
    }
}
