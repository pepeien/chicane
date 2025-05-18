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
            static constexpr const char* TAG_ID = "Character";

        public:
            Character();

        public:
            bool isDrawable() const override;

        protected:
            void refreshPrimitive() override;

        public:
            char getCharacter() const;
            void setCharacter(char inValue);

            bool hasFont() const;
            const Box::Font::ParsedData& getFont() const;

            bool hasGlyph() const;
            const Box::Font::Glyph& getGlyph() const;

        protected:
            void refreshFont();

        private:
            char m_character;
        };
    }
}