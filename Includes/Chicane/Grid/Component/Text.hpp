#pragma once

#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Text : public Component
        {
        public:
            // Tag
            static inline constexpr const char* TAG_ID = "Text";

        public:
            Text(const pugi::xml_node& inNode);

        protected:
            void onRefresh() override;

        public:
            const String& getText() const;
            void setText(const String& inValue);

        protected:
            void refreshText();

        private:
            String m_text;
            String m_parsedText;
        };
    }
}