#pragma once

#include <cstddef>
#include <vector>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        class SelectOption;

        CH_TYPE(Manual)
        class CHICANE_GRID Select : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Select";

            // Properties
            static constexpr inline const char* VALUE_ATTRIBUTE_NAME       = "value";
            static constexpr inline const char* PLACEHOLDER_ATTRIBUTE_NAME = "placeholder";
            static constexpr inline const char* ON_INPUT_ATTRIBUTE_NAME    = "onInput";

        public:
            CH_CONSTRUCTOR()
            Select(const pugi::xml_node& inNode);

        public:
            bool isFocusable() const override;

            bool onEvent(const WindowEvent& inEvent) override;
            void tick(float inDeltaTime) override;

        public:
            std::vector<SelectOption*> getOptions() const;
            std::vector<SelectOption*> getVisibleOptions() const;
            bool isHighlighted(const SelectOption* inOption) const;

        protected:
            void onTick(float inDeltaTime) override;
            void onFocus() override;
            void onBlur() override;

        public:
            CH_FUNCTION()
            void toggle();

            CH_FUNCTION()
            void open();

            CH_FUNCTION()
            void close();

            CH_FUNCTION()
            void choose(String inValue);

            void pick(SelectOption* inOption);

        private:
            void refreshValue();
            void refreshItems();
            void refreshLabel();
            void refreshHighlight();
            void commit();
            void emitInput();
            void moveHighlight(int inDelta);
            void chooseHighlighted();

        public:
            CH_FIELD()
            String value;

            CH_FIELD()
            String label;

            CH_FIELD()
            String placeholder;

            CH_FIELD()
            bool isOpen;

            CH_FIELD()
            std::vector<String> items;

        private:
            bool        m_bEdited;
            std::size_t m_highlighted;
        };
    }
}
