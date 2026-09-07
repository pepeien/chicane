#pragma once

#include "Chicane/Box/Font/Glyph.hpp"
#include "Chicane/Core/Color.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID TextGlyph : public Component
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Glyph";

            // Values
            static constexpr inline const char* QUAD_ID       = "Glyph_Quad";
            static constexpr inline const float QUAD_DILATION = 1.0f;

        public:
            TextGlyph();

        public:
            bool isDrawable() const override;

        protected:
            void refreshPrimitive() override;
            void refreshStyleRuleset() override;

        public:
            void configure(
                const Box::FontGlyph* inGlyph,
                float                 inFontSize,
                float                 inLetterSpacing,
                const Color::Rgba&    inColor,
                const Vec2&           inRelative,
                float                 inAscender
            );
            float getAdvance() const;
            const Vec2& getRelative() const;

            void clear();
            void syncPosition();

        private:
            const Box::FontGlyph* m_glyph;
            float                 m_advance;
            float                 m_dilation;
            Vec2                  m_relative;
        };
    }
}
