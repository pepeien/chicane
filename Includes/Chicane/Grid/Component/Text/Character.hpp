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
            bool hasGlyph() const;
            void setGlyph(const Box::FontGlyph* inValue);

        protected:
            void refreshGlyphStyle();

        private:
            const Box::FontGlyph* m_glyph;
        };
    }
}