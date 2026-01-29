#pragma once

#include "Chicane/Box/Font/Parsed.hpp"

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

            bool hasGlyph() const;
            const Box::FontGlyph& getGlyph() const;

        protected:
            void refreshFont();

        private:
            bool           m_bCanUpdatePrimitive;

            char           m_character;

            Box::FontGlyph m_glyph;
        };
    }
}