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
            std::uint64_t getFPS(Grid::ComponentEvent inEvent);
            std::string getFrametime(Grid::ComponentEvent inEvent);

            int addActor(Grid::ComponentEvent inEvent);
        };
    }
}