#include "Chicane/Renderer/Queue/FamilyIndices.hpp"

namespace Chicane
{
    namespace Queue
    {
        bool FamilyIndices::isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    }
}