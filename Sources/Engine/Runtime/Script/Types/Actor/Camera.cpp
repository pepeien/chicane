#include "../Shared.hpp"

#include "Chicane/Runtime/Scene/Actor/Camera.hpp"

namespace Chicane
{
    namespace Types
    {
        static ACamera* checkCamera(lua_State* inState)
        {
            ACamera* camera = dynamic_cast<ACamera*>(checkActor(inState, 1));
            if (!camera)
            {
                luaL_error(inState, "expected ACamera");
            }

            return camera;
        }

        static int activate(lua_State* inState)
        {
            checkCamera(inState)->activate();

            return 0;
        }

        static int deactivate(lua_State* inState)
        {
            checkCamera(inState)->deactivate();

            return 0;
        }

        static const luaL_Reg kMethods[] = {
            {"activate",   activate  },
            {"deactivate", deactivate},
            {nullptr,      nullptr   }
        };

        void bindACamera(lua_State* inState)
        {
            bindMetatable(inState, ACAMERA_MT, kMethods, ACTOR_MT);
        }
    }
}
