#include "../Shared.hpp"

#include "Chicane/Runtime/Scene/Actor/Sky.hpp"

namespace Chicane
{
    namespace Types
    {
        static const luaL_Reg kMethods[] = {
            {nullptr, nullptr}
        };

        void bindASky(lua_State* inState)
        {
            bindMetatable(inState, ASKY_MT, kMethods, ACTOR_MT);
        }
    }
}
