#pragma once

#include "Chicane/Box/Font.hpp"

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Text : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Chicane::Grid::Text";

        public:
            CH_CONSTRUCTOR()
            Text(const pugi::xml_node& inNode);

        protected:
            void onRefresh() override;

        public:
            const String& getText() const;
            void setText(const String& inValue);

        private:
            bool hasFont() const;

            void refreshFont();
            void refreshText();

        private:
            String           m_text;
            String           m_parsedText;

            const Box::Font* m_font;
        };
    }
}