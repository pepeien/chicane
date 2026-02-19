#pragma once

#include "Chicane/Box/Font/Parsed.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID TextCharacter : public Component
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Character";

        public:
            TextCharacter();

        public:
            bool isDrawable() const override;

        protected:
            void refreshPrimitive() override;

            void onRefresh() override;

        public:
            void disable();

            bool hasCharacter() const;
            char32_t getCharacter() const;
            void setCharacter(char32_t inValue);

            bool hasGlyph() const;
            const Box::FontGlyph& getGlyph() const;

        protected:
            void refreshFont();
            void refreshFontStyle();

        private:
            char32_t       m_character;

            Box::FontGlyph m_glyph;
        };
    }
}