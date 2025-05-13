#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Component.hpp"

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

        public:
            void onEvent(const SDL_Event& inEvent);

            Window::Instance* getWindow() const;
            void setWindow(Window::Instance* inWindow);

            std::vector<const Component*> getDrawableChildren() const;
            std::vector<const Component*> flatChildren() const;

        private:
            std::vector<const Component*> flatChildren(const Component* inParent) const;

        protected:
            Window::Instance*  m_window;

            pugi::xml_document m_document;

            Style::Sources     m_styles;
        };
    }
}