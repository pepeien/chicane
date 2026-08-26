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
            static constexpr inline const char* TAG_ID        = "Glyph";
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
                float                 inPenX,
                float                 inLineTop,
                float                 inAscender
            );
            float getAdvance() const;
            float getRelativeX() const;
            float getRelativeY() const;

            void clear();
            void syncPosition();

        private:
            const Box::FontGlyph* m_glyph;
            float                 m_advance;
            float                 m_dilation;
            float                 m_relativeX;
            float                 m_relativeY;
        };
    }
}
