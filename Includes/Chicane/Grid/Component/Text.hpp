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

        public:
            void onTick(float inDelta) override;

        public:
            const std::string& getText() const;
            void setText(const std::string& inValue);

        private:
            std::string m_text;
        };
    }
}