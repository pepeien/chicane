#include "../Shared.hpp"

#include "Chicane/Runtime/Scene/Actor/Pawn.hpp"

namespace Chicane
{
    namespace Types
    {
        static int isControlled(lua_State* inState)
        {
            APawn* pawn = dynamic_cast<APawn*>(checkActor(inState, 1));
            if (!pawn)
            {
                luaL_error(inState, "expected APawn");

                return 0;
            }

            lua_pushboolean(inState, pawn->isControlled());

            return 1;
        }

        static const luaL_Reg kMethods[] = {
            {"isControlled", isControlled},
            {nullptr,        nullptr     }
        };

        void bindAPawn(lua_State* inState)
        {
            bindMetatable(inState, APAWN_MT, kMethods, ACTOR_MT);
        }
    }
}
