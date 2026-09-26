#include "../Shared.hpp"

namespace Chicane
{
    namespace Types
    {
        static const luaL_Reg kMethods[] = {
            {nullptr, nullptr}
        };

        void bindCCamera(lua_State* inState)
        {
            bindMetatable(inState, CCAMERA_MT, kMethods, CVIEW_MT);
        }
    }
}
