#include "../Shared.hpp"

#include "Chicane/Runtime/Scene/Component/Sound.hpp"

namespace Chicane
{
    namespace Types
    {
        static CSound* checkSound(lua_State* inState)
        {
            CSound* sound = dynamic_cast<CSound*>(checkComponent(inState, 1));
            if (!sound)
            {
                luaL_error(inState, "expected CSound");
            }

            return sound;
        }

        static int load(lua_State* inState)
        {
            checkSound(inState)->load(luaL_checkstring(inState, 2));

            return 0;
        }

        static int play(lua_State* inState)
        {
            checkSound(inState)->play();

            return 0;
        }

        static const luaL_Reg kMethods[] = {
            {"load",  load   },
            {"play",  play   },
            {nullptr, nullptr}
        };

        void bindCSound(lua_State* inState)
        {
            bindMetatable(inState, CSOUND_MT, kMethods, COMPONENT_MT);
        }
    }
}
