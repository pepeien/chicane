#pragma once

#include "Core.hpp"
#include "Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE View : public Component
        {
        public:
            static constexpr const char* TAG_ID = "View";

        public:
            View(const std::string& inId, const std::string& inSource);

            virtual ~View() = default;

        public:
            void onChildAddition(Component* inChild) override;

        protected:
            pugi::xml_document m_document;

            Style::Sources m_styles;
        };
    }
}