#pragma once

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Cursor.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID View : public Component
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "View";

            // Attribute
            static constexpr inline const char* PATH_ATTRIBUTE_NAME = "path";

        public:
            CH_CONSTRUCTOR()
            View();

            CH_CONSTRUCTOR()
            View(const FileSystem::Path& inTemplate, const FileSystem::Path& inStyle = {});

        public:
            // Window
            void handle(const WindowEvent& inEvent);
            WindowCursor getPointer() const;
            void clearInteraction(Component* inComponent);
            void focusOn(Component* inComponent);

            // Children
            std::vector<Component*> getChildrenAt(const Vec2& inLocation) const;

            // Styling
            const StyleFile& getStyleFile() const;

        protected:
            void load(const FileSystem::Path& inTemplate, const FileSystem::Path& inStyle = {});
            Component* resolveHit(Component* inHit) const;
            Component* resolveFocus(Component* inHit) const;
            void syncHovered(Component* inComponent);
            void syncFocused(Component* inComponent);

        protected:
            // Routing
            String     m_path;

            // Interaction
            Component* m_hovered;
            Component* m_focused;
        };
    }
}