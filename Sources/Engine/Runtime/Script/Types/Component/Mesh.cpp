#include "../Shared.hpp"

#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

namespace Chicane
{
    namespace Types
    {
        static CMesh* checkMesh(lua_State* inState)
        {
            CMesh* mesh = dynamic_cast<CMesh*>(checkComponent(inState, 1));
            if (!mesh)
            {
                luaL_error(inState, "expected CMesh");
            }

            return mesh;
        }

        static int setMesh(lua_State* inState)
        {
            checkMesh(inState)->setMesh(luaL_checkstring(inState, 2));

            return 0;
        }

        static int hasMesh(lua_State* inState)
        {
            lua_pushboolean(inState, checkMesh(inState)->hasMesh());

            return 1;
        }

        static int playAnimation(lua_State* inState)
        {
            checkMesh(inState)->playAnimation(luaL_checkstring(inState, 2));

            return 0;
        }

        static int queueAnimation(lua_State* inState)
        {
            checkMesh(inState)->queueAnimation(luaL_checkstring(inState, 2));

            return 0;
        }

        static int stopAnimation(lua_State* inState)
        {
            checkMesh(inState)->stopAnimation();

            return 0;
        }

        static int isPlayingAnimation(lua_State* inState)
        {
            lua_pushboolean(inState, checkMesh(inState)->isPlayingAnimation());

            return 1;
        }

        static int getPlayingAnimation(lua_State* inState)
        {
            lua_pushstring(inState, checkMesh(inState)->getPlayingAnimation().toChar());

            return 1;
        }

        static int setCanCastShadows(lua_State* inState)
        {
            checkMesh(inState)->setCanCastShadows(lua_toboolean(inState, 2) != 0);

            return 0;
        }

        static int setIsLit(lua_State* inState)
        {
            checkMesh(inState)->setIsLit(lua_toboolean(inState, 2) != 0);

            return 0;
        }

        static const luaL_Reg kMethods[] = {
            {"setMesh",             setMesh            },
            {"hasMesh",             hasMesh            },
            {"playAnimation",       playAnimation      },
            {"queueAnimation",      queueAnimation     },
            {"stopAnimation",       stopAnimation      },
            {"isPlayingAnimation",  isPlayingAnimation },
            {"getPlayingAnimation", getPlayingAnimation},
            {"setCanCastShadows",   setCanCastShadows  },
            {"setIsLit",            setIsLit           },
            {nullptr,               nullptr            }
        };

        void bindCMesh(lua_State* inState)
        {
            bindMetatable(inState, CMESH_MT, kMethods, COMPONENT_MT);
        }
    }
}
