#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID View : public Component
        {
        public:
            static constexpr inline const char* TAG_ID = "View";

            static constexpr inline const char* PATH_ATTRIBUTE_NAME = "path";

        public:
            View(const std::string& inSource);

            virtual ~View() = default;

        protected:
            virtual void onActivation() { return; }
            virtual void onDeactivation() { return; }

        protected:
            void onAdopted(Component* inChild) override;

        public:
            // Lifecycle
            void activate();
            void deactivate();

            const std::string& getPath() const;

        protected:
            std::string         m_path;

            Style::Source::List m_styles;
        };
    }
}