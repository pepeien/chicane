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
            using Glyphs   = std::unordered_map<char32_t, FontGlyph>;
            using Kernings = std::unordered_map<std::uint64_t, float>;

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

            float getAscender() const;
            float getDescender() const;
            float getLineHeight() const;

            void setMetrics(float inAscender, float inDescender);

            float getKerning(char32_t inLeft, char32_t inRight) const;
            void addKerning(char32_t inLeft, char32_t inRight, float inValue);

            const Glyphs& getGlyphs() const;
            const FontGlyph& getGlyph(char inCharacter) const;
            const FontGlyph& getGlyph(char32_t inCode) const;

            void addGlyph(const Glyphs& inValue);
            void addGlyph(const FontGlyph& inValue);

        private:
            static std::uint64_t kerningKey(char32_t inLeft, char32_t inRight);

        private:
            String   m_name;
            Glyphs   m_glyphs;

            float    m_ascender  = 0.0f;
            float    m_descender = 0.0f;

            Kernings m_kernings;
        };
    }
}