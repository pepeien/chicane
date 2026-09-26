#include "Chicane/Core/Script/Types.hpp"

namespace Chicane
{
    namespace Script
    {
        namespace Types
        {
            void bind(lua_State* inState)
            {
                bindVec2(inState);
                bindVec3(inState);
                bindVec4(inState);
                bindColor(inState);
                bindRotator(inState);
            }
        }
    }
}
