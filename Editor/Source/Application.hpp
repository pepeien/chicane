#pragma once

#include <string>

#include "Core/Application.hpp"
#include "Core/Log.hpp"
#include "Renderer/Level/Instance.hpp"

namespace Chicane
{
    namespace Editor
    {
        void initLevel(Level::Instance& outLevel);

        int run();
    }
}
