#pragma once

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

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
            static constexpr inline const char* TAG_ID = "Chicane::Grid::View";

            static constexpr inline const char* PATH_ATTRIBUTE_NAME = "path";

        public:
            CH_CONSTRUCTOR()
            View(const FileSystem::Path& inSource);

        public:
            // Window
            void handle(const WindowEvent& inEvent);

            // Children
            std::vector<Component*> getChildrenAt(const Vec2& inLocation) const;

            // Styling
            const StyleFile& getStyleFile() const;
            void importStyleFile(const FileSystem::Path& inValue);

        protected:
            // Routing
            String    m_path;

            // Styling
            StyleFile m_styles;
        };
    }
}