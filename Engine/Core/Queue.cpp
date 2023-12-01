#include "Queue.h"

namespace Engine
{
    namespace Core
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