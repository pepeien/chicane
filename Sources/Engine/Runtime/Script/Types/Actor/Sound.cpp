#include "../Shared.hpp"

#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Runtime/Scene/Actor/Sound.hpp"

namespace Chicane
{
    namespace Types
    {
        static ASound* checkSound(lua_State* inState)
        {
            ASound* sound = dynamic_cast<ASound*>(checkActor(inState, 1));
            if (!sound)
            {
                luaL_error(inState, "expected ASound");
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

        void bindASound(lua_State* inState)
        {
            bindMetatable(inState, ASOUND_MT, kMethods, ACTOR_MT);
        }
    }
}
