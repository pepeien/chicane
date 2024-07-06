#pragma once

#include "Runtime/Core.hpp"
#include "Runtime/Grid.hpp"

namespace Chicane
{
    namespace Editor
    {
        enum class Menu
        {
            None,
            Grid
        };

        enum class ToolMenu
        {
            None,
            Cubemap,
            Model
        };

        class View : public Grid::View
        {
        public:
            View();

        public:
            void show(
                const Vec2& inResolution,
                const Vec2& inPosition
            ) override;

        private:
            void onMetricsButtonClick();
            void onGridButtonClick();

            // Tool Menus
            void renderCubemapToolMenu();
            void renderModelToolMenu();

        private:
            bool m_isShowingMetrics;

            Menu m_activeMenu;
            ToolMenu m_activeToolMenu;

            // Cubemap
            std::vector<FileSystem::FileFormat> m_cubemapEntryFormats;
            std::vector<std::string> m_cubemapAxes;

            char* m_cubemapName;
            Box::WriteInfo m_cubemapInfo;
        };
    }
}