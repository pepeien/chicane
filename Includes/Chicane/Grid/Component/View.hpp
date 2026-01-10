#pragma once

#include "Chicane/Core/String.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID View : public Component
        {
        public:
            static inline constexpr const char* TAG_ID              = "View";

            static inline constexpr const char* PATH_ATTRIBUTE_NAME = "path";

        public:
            View(const String& inSource);

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

            const String& getPath() const;

        protected:
            String            m_path;

            StyleSource::List m_styles;
        };
    }
}