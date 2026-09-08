#include "Chicane/Kerb/Collision/Group.hpp"

namespace Chicane
{
    namespace Kerb
    {
        bool CollisionGroup::isValid() const
        {
            return group != Invalid;
        }
    }
}