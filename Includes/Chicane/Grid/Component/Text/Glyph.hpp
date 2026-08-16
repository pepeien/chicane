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
            static constexpr inline const char* TAG_ID = "Glyph";

        public:
            TextGlyph();

        public:
            bool isDrawable() const override;

        protected:
            void refreshPrimitive() override;

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

            void clear();
            void syncPosition();

        private:
            const Box::FontGlyph* m_glyph;
            float                 m_advance;
            float                 m_relativeX;
            float                 m_relativeY;
        };
    }
}
