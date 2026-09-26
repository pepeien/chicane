#include "../Shared.hpp"

#include "Chicane/Runtime/Scene/Actor/Light.hpp"

namespace Chicane
{
    namespace Types
    {
        static ALight* checkLight(lua_State* inState)
        {
            ALight* light = dynamic_cast<ALight*>(checkActor(inState, 1));
            if (!light)
            {
                luaL_error(inState, "expected ALight");
            }

            return light;
        }

        static int getLight(lua_State* inState)
        {
            ALight* light = checkLight(inState);
            if (!light->light)
            {
                lua_pushnil(inState);

                return 1;
            }

            pushComponent(inState, light->light);

            return 1;
        }

        static const luaL_Reg kMethods[] = {
            {"getLight", getLight},
            {nullptr,    nullptr }
        };

        void bindALight(lua_State* inState)
        {
            bindMetatable(inState, ALIGHT_MT, kMethods, ACTOR_MT);
        }
    }
}
