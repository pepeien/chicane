#include "Chicane/Runtime/Renderer/Vulkan/Surface.hpp"

#include <SDL3/SDL_vulkan.h>

namespace Chicane
{
    namespace Vulkan
    {
        namespace Surface
        {
            void init(vk::SurfaceKHR& outSurface, const vk::Instance& inInstance, void* inWindow)
            {
                VkSurfaceKHR c_style_surface;

                if (!SDL_Vulkan_CreateSurface(
                        static_cast<SDL_Window*>(inWindow),
                        inInstance,
                        nullptr,
                        &c_style_surface
                    ))
                {
                    throw std::runtime_error(SDL_GetError());
                }

                outSurface = vk::SurfaceKHR(c_style_surface);
            }
        }
    }
}