#include "Chicane/Grid/Script/Types.hpp"

struct lua_State;

namespace Chicane
{
    namespace Grid
    {
        namespace Types
        {
            void bind(lua_State* inState)
            {
                bindComponent(inState);
            }
        }
    }
}
