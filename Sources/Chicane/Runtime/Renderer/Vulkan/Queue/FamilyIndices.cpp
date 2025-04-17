#include "Runtime/Renderer/Vulkan/Queue/FamilyIndices.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Queue
        {
            bool FamilyIndices::isComplete()
            {
                return graphicsFamily.has_value() && presentFamily.has_value();
            }
        }
    }
}