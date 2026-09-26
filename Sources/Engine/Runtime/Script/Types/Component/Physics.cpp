#include "../Shared.hpp"

#include "Chicane/Runtime/Scene/Component/Physics.hpp"

namespace Chicane
{
    namespace Types
    {
        static CPhysics* checkPhysics(lua_State* inState)
        {
            CPhysics* physics = dynamic_cast<CPhysics*>(checkComponent(inState, 1));
            if (!physics)
            {
                luaL_error(inState, "expected CPhysics");
            }

            return physics;
        }

        static int moveTo(lua_State* inState)
        {
            checkPhysics(inState)->moveTo(checkVec3Arg(inState, 2));

            return 0;
        }

        static int moveBy(lua_State* inState)
        {
            checkPhysics(inState)->moveBy(checkVec3Arg(inState, 2));

            return 0;
        }

        static int getLinearVelocity(lua_State* inState)
        {
            Script::Types::pushVec3(inState, checkPhysics(inState)->getLinearVelocity());

            return 1;
        }

        static int setLinearVelocity(lua_State* inState)
        {
            checkPhysics(inState)->setLinearVelocity(checkVec3Arg(inState, 2));

            return 0;
        }

        static int addImpulse(lua_State* inState)
        {
            const bool bDirectionVec = Script::Types::isVec3(inState, 2);
            const Vec3 direction     = checkVec3Arg(inState, 2);
            const int  forceIndex    = bDirectionVec ? 3 : 5;
            const Vec3 location      = checkVec3Arg(inState, forceIndex + 1);
            checkPhysics(inState)
                ->addImpulse(direction, static_cast<float>(luaL_checknumber(inState, forceIndex)), location);

            return 0;
        }

        static int setMass(lua_State* inState)
        {
            checkPhysics(inState)->setMass(static_cast<float>(luaL_checknumber(inState, 2)));

            return 0;
        }

        static int getGravityFactor(lua_State* inState)
        {
            lua_pushnumber(inState, static_cast<lua_Number>(checkPhysics(inState)->getGravityFactor()));

            return 1;
        }

        static int setGravityFactor(lua_State* inState)
        {
            checkPhysics(inState)->setGravityFactor(static_cast<float>(luaL_checknumber(inState, 2)));

            return 0;
        }

        static const luaL_Reg kMethods[] = {
            {"moveTo",            moveTo           },
            {"moveBy",            moveBy           },
            {"getLinearVelocity", getLinearVelocity},
            {"setLinearVelocity", setLinearVelocity},
            {"addImpulse",        addImpulse       },
            {"setMass",           setMass          },
            {"getGravityFactor",  getGravityFactor },
            {"setGravityFactor",  setGravityFactor },
            {nullptr,             nullptr          }
        };

        void bindCPhysics(lua_State* inState)
        {
            bindMetatable(inState, CPHYSICS_MT, kMethods, COMPONENT_MT);
        }
    }
}
