#include "../Shared.hpp"

#include "Chicane/Runtime/Scene/Component/View.hpp"

namespace Chicane
{
    namespace Types
    {
        static CView* checkView(lua_State* inState)
        {
            CView* view = dynamic_cast<CView*>(checkComponent(inState, 1));
            if (!view)
            {
                luaL_error(inState, "expected CView");
            }

            return view;
        }

        static int getFieldOfView(lua_State* inState)
        {
            lua_pushnumber(inState, static_cast<lua_Number>(checkView(inState)->getFieldOfView()));

            return 1;
        }

        static int setFieldOfView(lua_State* inState)
        {
            checkView(inState)->setFieldOfView(static_cast<float>(luaL_checknumber(inState, 2)));

            return 0;
        }

        static int getAspectRatio(lua_State* inState)
        {
            lua_pushnumber(inState, static_cast<lua_Number>(checkView(inState)->getAspectRatio()));

            return 1;
        }

        static int getNearClip(lua_State* inState)
        {
            lua_pushnumber(inState, static_cast<lua_Number>(checkView(inState)->getNearClip()));

            return 1;
        }

        static int getFarClip(lua_State* inState)
        {
            lua_pushnumber(inState, static_cast<lua_Number>(checkView(inState)->getFarClip()));

            return 1;
        }

        static int setClip(lua_State* inState)
        {
            checkView(inState)->setClip(
                static_cast<float>(luaL_checknumber(inState, 2)),
                static_cast<float>(luaL_checknumber(inState, 3))
            );

            return 0;
        }

        static int getFocusPoint(lua_State* inState)
        {
            Script::Types::pushVec3(inState, checkView(inState)->getFocusPoint());

            return 1;
        }

        static int setFocusPoint(lua_State* inState)
        {
            checkView(inState)->setFocusPoint(checkVec3Arg(inState, 2));

            return 0;
        }

        static int getTarget(lua_State* inState)
        {
            lua_pushstring(inState, checkView(inState)->getTarget().toChar());

            return 1;
        }

        static int setTarget(lua_State* inState)
        {
            checkView(inState)->setTarget(luaL_checkstring(inState, 2));

            return 0;
        }

        static const luaL_Reg kMethods[] = {
            {"getFieldOfView", getFieldOfView},
            {"setFieldOfView", setFieldOfView},
            {"getAspectRatio", getAspectRatio},
            {"getNearClip",    getNearClip   },
            {"getFarClip",     getFarClip    },
            {"setClip",        setClip       },
            {"getFocusPoint",  getFocusPoint },
            {"setFocusPoint",  setFocusPoint },
            {"getTarget",      getTarget     },
            {"setTarget",      setTarget     },
            {nullptr,          nullptr       }
        };

        void bindCView(lua_State* inState)
        {
            bindMetatable(inState, CVIEW_MT, kMethods, COMPONENT_MT);
        }
    }
}
