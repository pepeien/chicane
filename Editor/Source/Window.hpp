#pragma once

#include "Core.hpp"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

namespace Editor
{
    namespace Window
    {
        void init(Engine::Window* inEngineWindow);
    }
}