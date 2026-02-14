#pragma once

#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Style/File.hpp"

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
            View(const String& inSource);

        protected:
            virtual void onActivation() { return; }
            virtual void onDeactivation() { return; }

        protected:
            void onAdopted(Component* inChild) override;

        public:
            // Lifecycle
            void activate();
            void deactivate();

            // Window
            void setWindow(Window* inWindow);

            // Children
            std::vector<Component*> getFlatChildren(const Component* inParent = nullptr) const;
            std::vector<Component*> getChildrenOn(const Vec2& inLocation) const;

            // Styling
            const StyleFile& getStyleFile() const;
            void importStyleFile(const FileSystem::Path& inValue);

        protected:
            // Window
            void handle(const WindowEvent& inEvent);

        protected:
            // Window
            const Window*           m_window;
            WindowEventSubscription m_windowEventSubscription;

            // Routing
            String                  m_path;

            // Styling
            StyleFile               m_styles;
        };
    }
}