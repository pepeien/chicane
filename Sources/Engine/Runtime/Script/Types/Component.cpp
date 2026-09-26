#include "Chicane/Runtime/Script/Types.hpp"

#include "Chicane/Core/Script/Types.hpp"

#include "Shared.hpp"

#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"
#include "Chicane/Runtime/Scene/Component/Physics.hpp"
#include "Chicane/Runtime/Scene/Component/Sound.hpp"
#include "Chicane/Runtime/Scene/Component/View.hpp"

namespace Chicane
{
    namespace Types
    {
        static const char* const kComponentMetatables[] = {
            CCAMERA_MT, CLIGHT_MT, CVIEW_MT, CMESH_MT, CPHYSICS_MT, CSOUND_MT, COMPONENT_MT
        };

        static ComponentBox* testComponent(lua_State* inState, int inIndex)
        {
            for (const char* metatable : kComponentMetatables)
            {
                if (ComponentBox* box = static_cast<ComponentBox*>(luaL_testudata(inState, inIndex, metatable)))
                {
                    return box;
                }
            }

            return nullptr;
        }

        static int getId(lua_State* inState)
        {
            lua_pushstring(inState, checkComponent(inState, 1)->getId().toChar());

            return 1;
        }

        static int setId(lua_State* inState)
        {
            checkComponent(inState, 1)->setId(luaL_checkstring(inState, 2));

            return 0;
        }

        static int getTranslation(lua_State* inState)
        {
            Script::Types::pushVec3(inState, checkComponent(inState, 1)->getAbsoluteTranslation());

            return 1;
        }

        static int setTranslation(lua_State* inState)
        {
            checkComponent(inState, 1)->setAbsoluteTranslation(checkVec3Arg(inState, 2));

            return 0;
        }

        static int lookAt(lua_State* inState)
        {
            checkComponent(inState, 1)->lookAt(checkVec3Arg(inState, 2));

            return 0;
        }

        static int isActive(lua_State* inState)
        {
            lua_pushboolean(inState, checkComponent(inState, 1)->isActive());

            return 1;
        }

        static int activate(lua_State* inState)
        {
            checkComponent(inState, 1)->activate();

            return 0;
        }

        static int deactivate(lua_State* inState)
        {
            checkComponent(inState, 1)->deactivate();

            return 0;
        }

        static int getParent(lua_State* inState)
        {
            Object* parent = checkComponent(inState, 1)->getParent();
            if (Actor* actor = dynamic_cast<Actor*>(parent))
            {
                pushActor(inState, actor);

                return 1;
            }

            if (Component* component = dynamic_cast<Component*>(parent))
            {
                pushComponent(inState, component);

                return 1;
            }

            lua_pushnil(inState);

            return 1;
        }

        static int getAttachments(lua_State* inState)
        {
            Component* component = checkComponent(inState, 1);
            lua_newtable(inState);

            int index = 1;
            for (Component* attachment : component->getAttachments())
            {
                if (!attachment)
                {
                    continue;
                }

                pushComponent(inState, attachment);
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
            {"isActive",       isActive      },
            {"activate",       activate      },
            {"deactivate",     deactivate    },
            {"getParent",      getParent     },
            {"getAttachments", getAttachments},
            {nullptr,          nullptr       }
        };

        void bindComponent(lua_State* inState)
        {
            bindMetatable(inState, COMPONENT_MT, kMethods);
            bindCView(inState);
            bindCCamera(inState);
            bindCLight(inState);
            bindCMesh(inState);
            bindCPhysics(inState);
            bindCSound(inState);
        }

        void pushComponent(lua_State* inState, Component* inComponent)
        {
            ComponentBox* box = static_cast<ComponentBox*>(lua_newuserdatauv(inState, sizeof(ComponentBox), 0));
            box->component    = inComponent;

            const char* metatable = COMPONENT_MT;
            if (dynamic_cast<CCamera*>(inComponent))
            {
                metatable = CCAMERA_MT;
            }
            else if (dynamic_cast<CLight*>(inComponent))
            {
                metatable = CLIGHT_MT;
            }
            else if (dynamic_cast<CView*>(inComponent))
            {
                metatable = CVIEW_MT;
            }
            else if (dynamic_cast<CMesh*>(inComponent))
            {
                metatable = CMESH_MT;
            }
            else if (dynamic_cast<CPhysics*>(inComponent))
            {
                metatable = CPHYSICS_MT;
            }
            else if (dynamic_cast<CSound*>(inComponent))
            {
                metatable = CSOUND_MT;
            }

            luaL_setmetatable(inState, metatable);
        }

        bool isComponent(lua_State* inState, int inIndex)
        {
            return testComponent(inState, inIndex) != nullptr;
        }

        Component* checkComponent(lua_State* inState, int inIndex)
        {
            ComponentBox* box = testComponent(inState, inIndex);
            if (!box)
            {
                luaL_error(inState, "expected Component");

                return nullptr;
            }

            if (!box->component)
            {
                luaL_error(inState, "component is no longer valid");

                return nullptr;
            }

            return box->component;
        }
    }
}
