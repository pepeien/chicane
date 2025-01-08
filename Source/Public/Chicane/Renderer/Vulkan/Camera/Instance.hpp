#pragma once

#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Camera/Data.hpp"
#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Buffer.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Camera
        {
            struct Instance
            {
            public:
                size_t                allocationSize;
                void*                 writeLocation;
                Buffer::Instance      buffer;
                Chicane::Camera::Data instance;
            };
        }
    }
}