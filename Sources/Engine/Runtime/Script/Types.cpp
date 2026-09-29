#include "Chicane/Runtime/Script/Types.hpp"

#include "Types/ReflectedValue.hpp"

namespace Chicane
{
    namespace Types
    {
        void bind(lua_State* inState)
        {
            bindReflectedStatics(inState);
        }
    }
}
