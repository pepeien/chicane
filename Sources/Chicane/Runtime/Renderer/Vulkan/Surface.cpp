#include "Chicane/Runtime/Renderer/Vulkan/Surface.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Surface
        {
            void init(
                vk::SurfaceKHR& outSurface,
                const vk::Instance& inInstance,
                SDL_Window* inWindow
            )
            {
                VkSurfaceKHR c_style_surface;
        
                if (
                    !SDL_Vulkan_CreateSurface(
                        inWindow,
                        inInstance,
                        nullptr,
                        &c_style_surface
                    )
                )
                {
                    throw std::runtime_error(SDL_GetError());
                }
        
                outSurface = vk::SurfaceKHR(c_style_surface);
            }
        } 
    }  
}