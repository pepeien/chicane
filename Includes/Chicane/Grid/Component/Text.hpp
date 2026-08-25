#pragma once

#include "Chicane/Box/Font.hpp"

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Scrollable.hpp"
#include "Chicane/Grid/Component/Text/Glyph.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Text : public Scrollable
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Text";

        public:
            CH_CONSTRUCTOR()
            Text(const pugi::xml_node& inNode);

            ~Text() override;

        public:
            void refresh() override;

            std::vector<Component*> getChildrenFlat() const override;

        protected:
            void onRefresh() override;

            void refreshSize() override;

        public:
            const String& getText() const;
            void setText(const String& inValue);

            const Vec2& getContentSize() const;

        private:
            bool hasFont() const;

            void refreshFont();
            void refreshText();
            void applyContentSize();
            void syncGlyphs();
            TextGlyph* acquireGlyph(std::size_t inIndex);

        private:
            String                  m_text;
            String                  m_parsedText;
            String                  m_layoutSignature;
            const Box::Font*        m_font;
            Vec2                    m_contentSize;
            std::vector<TextGlyph*> m_glyphs;
        };
    }
}