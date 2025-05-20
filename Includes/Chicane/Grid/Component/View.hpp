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
            static constexpr const char* TAG_ID = "View";

            static constexpr const char* PATH_ATTRIBUTE_NAME = "path";

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

            Window::Instance* getWindow() const;
            void setWindow(Window::Instance* inWindow);

            std::vector<const Component*> getDrawableChildren() const;
            std::vector<const Component*> flatChildren() const;

        private:
            std::vector<const Component*> flatChildren(const Component* inParent) const;

        protected:
            std::string        m_path;

            Window::Instance*  m_window;

            Style::Sources     m_styles;
        };
    }
}