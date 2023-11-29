#include "Queue.h"

namespace Engine
{
    namespace Core
    {
        bool QueueFamilyIndices::isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    }
}