#pragma once

#include "Runtime/Core.hpp"
#include "Runtime/Grid.hpp"

namespace Chicane
{
    namespace Editor
    {
        class View : public Grid::View
        {
        public:
            View();

        private:
            std::string getFPS(Grid::ComponentEvent inEvent);
            std::string getFrametime(Grid::ComponentEvent inEvent);
        };
    }
}