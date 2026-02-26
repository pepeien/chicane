#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Font/Glyph.hpp"

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX FontFamily
        {
        public:
            using Glyphs = std::unordered_map<char32_t, FontGlyph>;

        public:
            static const FontFamily& empty()
            {
                static const FontFamily result;

                return result;
            }

        public:
            FontFamily();

        public:
            bool hasGlyph(char inCharacter) const;
            bool hasGlyph(char32_t inCode) const;

            const String& getName() const;
            void setName(const String& inValue);

            const Glyphs& getGlyphs() const;
            const FontGlyph& getGlyph(char inCharacter) const;
            const FontGlyph& getGlyph(char32_t inCode) const;

            void addGlyph(const Glyphs& inValue);
            void addGlyph(const FontGlyph& inValue);

        private:
            String m_name;
            Glyphs m_glyphs;
        };
    }
}