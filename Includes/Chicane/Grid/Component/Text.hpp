#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Text : public Component
        {
        public:
            // Tag
            static constexpr const char* TAG_ID = "Text";

        public:
            Text(const pugi::xml_node& inNode);

        protected:
            void onTick(float inDelta) override;

        public:
            const std::string& getText() const;
            void setText(const std::string& inValue);

        protected:
            void refreshText();

        private:
            std::string m_text;
            std::string m_lastText;
        };
    }
}