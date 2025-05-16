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

            std::vector<Vertex> getPrimitive() const override;

        public:
            char getCharacter() const;
            void setCharacter(char inValue);

            bool hasFont() const;
            const Box::Font::RawData& getFont() const;

            bool hasGlyph() const;
            const Box::Font::Glyph& getGlyph() const;

        private:
            void refreshStyle();

        private:
            char m_character;
        };
    }
}