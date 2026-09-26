#include "../../Shared.hpp"

#include "Chicane/Runtime/Scene/Actor/Pawn/Character.hpp"

namespace Chicane
{
    namespace Types
    {
        static ACharacter* checkCharacter(lua_State* inState)
        {
            ACharacter* character = dynamic_cast<ACharacter*>(checkActor(inState, 1));
            if (!character)
            {
                luaL_error(inState, "expected ACharacter");
            }

            return character;
        }

        static int setMoveScale(lua_State* inState)
        {
            checkCharacter(inState)->setMoveScale(static_cast<float>(luaL_checknumber(inState, 2)));

            return 0;
        }

        static int setMoveInput(lua_State* inState)
        {
            checkCharacter(inState)->setMoveInput(
                static_cast<float>(luaL_checknumber(inState, 2)),
                static_cast<float>(luaL_checknumber(inState, 3)),
                static_cast<float>(luaL_optnumber(inState, 4, 0.0))
            );

            return 0;
        }

        static int move(lua_State* inState)
        {
            checkCharacter(inState)->move(
                checkVec3Arg(inState, 2),
                static_cast<float>(luaL_checknumber(inState, Script::Types::isVec3(inState, 2) ? 3 : 5))
            );

            return 0;
        }

        static int jump(lua_State* inState)
        {
            checkCharacter(inState)->jump(
                static_cast<float>(luaL_optnumber(inState, 2, ACharacter::DEFAULT_JUMP_SPEED))
            );

            return 0;
        }

        static int addPitch(lua_State* inState)
        {
            checkCharacter(inState)->addPitch(static_cast<float>(luaL_checknumber(inState, 2)));

            return 0;
        }

        static int addYaw(lua_State* inState)
        {
            checkCharacter(inState)->addYaw(static_cast<float>(luaL_checknumber(inState, 2)));

            return 0;
        }

        static int addRoll(lua_State* inState)
        {
            checkCharacter(inState)->addRoll(static_cast<float>(luaL_checknumber(inState, 2)));

            return 0;
        }

        static const luaL_Reg kMethods[] = {
            {"setMoveScale", setMoveScale},
            {"setMoveInput", setMoveInput},
            {"move",         move        },
            {"jump",         jump        },
            {"addPitch",     addPitch    },
            {"addYaw",       addYaw      },
            {"addRoll",      addRoll     },
            {nullptr,        nullptr     }
        };

        void bindACharacter(lua_State* inState)
        {
            bindMetatable(inState, ACHARACTER_MT, kMethods, APAWN_MT);
        }
    }
}
