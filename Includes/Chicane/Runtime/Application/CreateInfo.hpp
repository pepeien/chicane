#pragma once

#include <functional>
#include <vector>

#include "Chicane/Core/FileSystem/Path.hpp"
#include "Chicane/Core/Window/Settings.hpp"

#include "Chicane/Renderer/Settings.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME ApplicationCreateInfo
    {
    public:
        // Window
        WindowSettings                window = {};

        // Renderer
        Renderer::Settings            renderer = {};

        // Modules
        std::vector<FileSystem::Path> modules = {};

        // Callbacks
        std::function<void()>         onSetup = nullptr;
    };
}