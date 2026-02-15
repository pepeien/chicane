#pragma once

#include "Chicane/Core/FileSystem.hpp"
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
            static constexpr inline const char* TAG_ID = "View";

            static constexpr inline const char* PATH_ATTRIBUTE_NAME = "path";

        public:
            View(const FileSystem::Path& inSource);

        public:
            // Children
            std::vector<Component*> getChildrenAt(const Vec2& inLocation) const;

            // Styling
            const StyleFile& getStyleFile() const;
            void importStyleFile(const FileSystem::Path& inValue);

        protected:
            // Window
            void handle(const WindowEvent& inEvent);

        protected:
            // Routing
            String    m_path;

            // Styling
            StyleFile m_styles;
        };
    }
}