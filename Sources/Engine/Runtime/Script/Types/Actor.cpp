#include "Chicane/Runtime/Script/Types.hpp"

#include "Chicane/Core/Script/Types.hpp"

#include "Shared.hpp"

#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Actor/Camera.hpp"
#include "Chicane/Runtime/Scene/Actor/Light.hpp"
#include "Chicane/Runtime/Scene/Actor/Pawn.hpp"
#include "Chicane/Runtime/Scene/Actor/Pawn/Character.hpp"
#include "Chicane/Runtime/Scene/Actor/Sky.hpp"
#include "Chicane/Runtime/Scene/Actor/Sound.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    namespace Types
    {
        static const char* const kActorMetatables[] = {
            ACHARACTER_MT, APAWN_MT, ACAMERA_MT, ALIGHT_MT, ASKY_MT, ASOUND_MT, ACTOR_MT
        };

        static ActorBox* testActor(lua_State* inState, int inIndex)
        {
            for (const char* metatable : kActorMetatables)
            {
                if (ActorBox* box = static_cast<ActorBox*>(luaL_testudata(inState, inIndex, metatable)))
                {
                    return box;
                }
            }

            return nullptr;
        }

        static int getId(lua_State* inState)
        {
            lua_pushstring(inState, checkActor(inState, 1)->getId().toChar());

            return 1;
        }

        static int setId(lua_State* inState)
        {
            checkActor(inState, 1)->setId(luaL_checkstring(inState, 2));

            return 0;
        }

        static int getTranslation(lua_State* inState)
        {
            Script::Types::pushVec3(inState, checkActor(inState, 1)->getAbsoluteTranslation());

            return 1;
        }

        static int setTranslation(lua_State* inState)
        {
            checkActor(inState, 1)->setAbsoluteTranslation(checkVec3Arg(inState, 2));

            return 0;
        }

        static int lookAt(lua_State* inState)
        {
            checkActor(inState, 1)->lookAt(checkVec3Arg(inState, 2));

            return 0;
        }

        static int getCenter(lua_State* inState)
        {
            Script::Types::pushVec3(inState, checkActor(inState, 1)->getCenter());

            return 1;
        }

        static int getSize(lua_State* inState)
        {
            Script::Types::pushVec3(inState, checkActor(inState, 1)->getBounds().getSize());

            return 1;
        }

        static int isTransient(lua_State* inState)
        {
            lua_pushboolean(inState, checkActor(inState, 1)->isTransient() ? 1 : 0);

            return 1;
        }

        static int getAttachments(lua_State* inState)
        {
            Actor* actor = checkActor(inState, 1);
            lua_newtable(inState);

            int index = 1;
            for (Component* component : actor->getAttachments())
            {
                if (!component)
                {
                    continue;
                }

                pushComponent(inState, component);
                lua_rawseti(inState, -2, index++);
            }

            return 1;
        }

        static const luaL_Reg kMethods[] = {
            {"getId",          getId         },
            {"setId",          setId         },
            {"getTranslation", getTranslation},
            {"setTranslation", setTranslation},
            {"lookAt",         lookAt        },
            {"getCenter",      getCenter     },
            {"getSize",        getSize       },
            {"isTransient",    isTransient   },
            {"getAttachments", getAttachments},
            {nullptr,          nullptr       }
        };

        void bindActor(lua_State* inState)
        {
            bindMetatable(inState, ACTOR_MT, kMethods);
            bindAPawn(inState);
            bindACharacter(inState);
            bindACamera(inState);
            bindALight(inState);
            bindASky(inState);
            bindASound(inState);
        }

        void pushActor(lua_State* inState, Actor* inActor)
        {
            ActorBox* box = static_cast<ActorBox*>(lua_newuserdatauv(inState, sizeof(ActorBox), 0));
            box->actor    = inActor;

            const char* metatable = ACTOR_MT;
            if (dynamic_cast<ACharacter*>(inActor))
            {
                metatable = ACHARACTER_MT;
            }
            else if (dynamic_cast<APawn*>(inActor))
            {
                metatable = APAWN_MT;
            }
            else if (dynamic_cast<ACamera*>(inActor))
            {
                metatable = ACAMERA_MT;
            }
            else if (dynamic_cast<ALight*>(inActor))
            {
                metatable = ALIGHT_MT;
            }
            else if (dynamic_cast<ASky*>(inActor))
            {
                metatable = ASKY_MT;
            }
            else if (dynamic_cast<ASound*>(inActor))
            {
                metatable = ASOUND_MT;
            }

            luaL_setmetatable(inState, metatable);
        }

        bool isActor(lua_State* inState, int inIndex)
        {
            return testActor(inState, inIndex) != nullptr;
        }

        Actor* checkActor(lua_State* inState, int inIndex)
        {
            ActorBox* box = testActor(inState, inIndex);
            if (!box)
            {
                luaL_error(inState, "expected Actor");

                return nullptr;
            }

            if (!box->actor)
            {
                luaL_error(inState, "actor is no longer valid");

                return nullptr;
            }

            return box->actor;
        }
    }
}
