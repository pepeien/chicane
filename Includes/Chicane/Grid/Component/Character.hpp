#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Component/Instance.hpp"

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

            void onRefresh() override;

        protected:
            void refreshPrimitive() override;

        public:
            void disable();

            bool hasCharacter() const;
            char getCharacter() const;
            void setCharacter(char inValue);

            bool hasFont() const;
            const Box::Font::Parsed& getFont() const;

            bool hasGlyph() const;
            const Box::Font::Glyph& getGlyph() const;

        protected:
            void refreshFont();

        private:
            bool m_canUpdate;

            char m_character;
        };
    }
}