#include "Chicane/Runtime/Script/Types.hpp"

struct lua_State;

namespace Chicane
{
    namespace Types
    {
        void bind(lua_State* inState)
        {
            bindActor(inState);
            bindComponent(inState);
            bindTraceRequest(inState);
            bindTrace(inState);
        }
    }
}
