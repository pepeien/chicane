#include "../Shared.hpp"

#include "Chicane/Runtime/Scene/Component/Light.hpp"

namespace Chicane
{
    namespace Types
    {
        static const luaL_Reg kMethods[] = {
            {nullptr, nullptr}
        };

        void bindCLight(lua_State* inState)
        {
            bindMetatable(inState, CLIGHT_MT, kMethods, CVIEW_MT);
        }
    }
}
