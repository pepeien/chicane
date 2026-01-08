#pragma once

#include "Chicane/Box/Asset/Font/Extracted.hpp"
#include "Chicane/Box/Asset/Font/Parsed.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Character : public Component
        {
        public:
            // Tag
            static inline constexpr const char* TAG_ID = "Character";

        public:
            Character();

        public:
            bool isDrawable() const override;

        protected:
            void refreshPrimitive() override;

            void onRefresh() override;

        public:
            void disable();

            bool hasCharacter() const;
            char getCharacter() const;
            void setCharacter(char inValue);

            bool hasFont() const;
            const Box::FontParsed& getFont() const;

            bool hasGlyph() const;
            const Box::FontGlyph& getGlyph() const;

        protected:
            void refreshFont();

        private:
            bool m_bCanUpdate;

            char m_character;
        };
    }
}