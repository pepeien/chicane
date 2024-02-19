#pragma once

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

        class EditorView : public Grid::View
        {
        public:
            EditorView();

        public:
            void show(
                const Vec2& inResolution,
                const Vec2& inPosition
            ) override;

        private:
            void onBoxButtonClick();

        private:
            Menu m_activeMenu;
        };
    }
}