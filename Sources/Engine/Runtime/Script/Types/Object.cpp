#include "Shared.hpp"

#include <typeinfo>

#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Script/Handle.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"
#include "Chicane/Runtime/Scene/Object.hpp"

namespace Chicane
{
    namespace Types
    {
        void* sOwnerSentinel()
        {
            static int sentinel = 0;

            return &sentinel;
        }

        static int collect(lua_State* inState)
        {
            if (ObjectBox* box = testObjectBox(inState, 1))
            {
                box->object = nullptr;
            }

            return 0;
        }

        static Object* liveObject(ObjectBox* inBox)
        {
            if (!inBox || !inBox->object || !Script::Handle::contains(inBox->object))
            {
                return nullptr;
            }

            return inBox->object;
        }

        static int getTranslation(lua_State* inState)
        {
            Script::Types::pushVec3(inState, checkObject(inState, 1)->getAbsoluteTranslation());

            return 1;
        }

        static int setTranslation(lua_State* inState)
        {
            checkObject(inState, 1)->setAbsoluteTranslation(checkVec3Arg(inState, 2));

            return 0;
        }

        static int lookAt(lua_State* inState)
        {
            checkObject(inState, 1)->lookAt(checkVec3Arg(inState, 2));

            return 0;
        }

        static int getCenter(lua_State* inState)
        {
            Script::Types::pushVec3(inState, checkObject(inState, 1)->getCenter());

            return 1;
        }

        static int getSize(lua_State* inState)
        {
            Script::Types::pushVec3(inState, checkObject(inState, 1)->getBounds().getSize());

            return 1;
        }

        static const luaL_Reg kMethods[] = {
            {"getTranslation", getTranslation},
            {"setTranslation", setTranslation},
            {"lookAt",         lookAt        },
            {"getCenter",      getCenter     },
            {"getSize",        getSize       },
            {nullptr,          nullptr       }
        };

        static void pushMethodTable(lua_State* inState)
        {
            if (lua_getfield(inState, LUA_REGISTRYINDEX, METHODS_REGISTRY) != LUA_TNIL)
            {
                return;
            }

            lua_pop(inState, 1);

            lua_newtable(inState);
            luaL_setfuncs(inState, kMethods, 0);

            lua_pushvalue(inState, -1);
            lua_setfield(inState, LUA_REGISTRYINDEX, METHODS_REGISTRY);
        }

        static String shortNameOf(const String& inName)
        {
            const std::size_t split = inName.lastOf(':');
            if (split == String::npos)
            {
                return inName;
            }

            return inName.substr(split + 1);
        }

        static void ensureMetatable(lua_State* inState, const char* inName)
        {
            if (luaL_getmetatable(inState, inName) != LUA_TNIL)
            {
                lua_pop(inState, 1);

                return;
            }

            lua_pop(inState, 1);

            luaL_newmetatable(inState, inName);

            lua_pushcfunction(inState, reflectedIndex);
            lua_setfield(inState, -2, "__index");

            lua_pushcfunction(inState, reflectedNewIndex);
            lua_setfield(inState, -2, "__newindex");

            lua_pushlightuserdata(inState, sOwnerSentinel());
            lua_setfield(inState, -2, OWNER_FIELD);

            const String shortName = shortNameOf(inName);

            lua_pushstring(inState, shortName.toChar());
            lua_setfield(inState, -2, "__name");

            lua_pushstring(inState, shortName.toChar());
            lua_setfield(inState, -2, "__metatable");

            lua_pushcfunction(inState, collect);
            lua_setfield(inState, -2, "__gc");

            lua_pop(inState, 1);
        }

        static const char* metatableNameOf(const Object* inObject)
        {
            const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(*inObject));
            if (!type || type->names.empty())
            {
                return OBJECT_MT;
            }

            return type->names.front().toChar();
        }

        void bindObject(lua_State* inState)
        {
            pushMethodTable(inState);
            lua_pop(inState, 1);

            ensureMetatable(inState, OBJECT_MT);
        }

        void pushObject(lua_State* inState, Object* inObject)
        {
            if (!inObject)
            {
                lua_pushnil(inState);

                return;
            }

            const char* metatable = metatableNameOf(inObject);
            ensureMetatable(inState, metatable);

            ObjectBox* box = static_cast<ObjectBox*>(lua_newuserdatauv(inState, sizeof(ObjectBox), 0));
            box->object    = inObject;

            luaL_setmetatable(inState, metatable);
        }

        ObjectBox* testObjectBox(lua_State* inState, int inIndex)
        {
            if (lua_type(inState, inIndex) != LUA_TUSERDATA || !lua_getmetatable(inState, inIndex))
            {
                return nullptr;
            }

            lua_pushstring(inState, OWNER_FIELD);
            lua_rawget(inState, -2);
            const bool bIsOwned = lua_touserdata(inState, -1) == sOwnerSentinel();
            lua_pop(inState, 2);

            if (!bIsOwned)
            {
                return nullptr;
            }

            return static_cast<ObjectBox*>(lua_touserdata(inState, inIndex));
        }

        bool isObject(lua_State* inState, int inIndex)
        {
            return liveObject(testObjectBox(inState, inIndex)) != nullptr;
        }

        Object* checkObject(lua_State* inState, int inIndex)
        {
            ObjectBox* box = testObjectBox(inState, inIndex);
            if (!box)
            {
                luaL_error(inState, "expected a scene object");

                return nullptr;
            }

            Object* object = liveObject(box);
            if (!object)
            {
                luaL_error(inState, "object is no longer valid");

                return nullptr;
            }

            return object;
        }

        void pushActor(lua_State* inState, Actor* inActor)
        {
            pushObject(inState, inActor);
        }

        bool isActor(lua_State* inState, int inIndex)
        {
            ObjectBox* box = testObjectBox(inState, inIndex);

            return dynamic_cast<Actor*>(liveObject(box)) != nullptr;
        }

        Actor* checkActor(lua_State* inState, int inIndex)
        {
            Actor* actor = dynamic_cast<Actor*>(checkObject(inState, inIndex));
            if (!actor)
            {
                luaL_error(inState, "expected Actor");

                return nullptr;
            }

            return actor;
        }

        void pushComponent(lua_State* inState, Component* inComponent)
        {
            pushObject(inState, inComponent);
        }

        bool isComponent(lua_State* inState, int inIndex)
        {
            ObjectBox* box = testObjectBox(inState, inIndex);

            return dynamic_cast<Component*>(liveObject(box)) != nullptr;
        }

        Component* checkComponent(lua_State* inState, int inIndex)
        {
            Component* component = dynamic_cast<Component*>(checkObject(inState, inIndex));
            if (!component)
            {
                luaL_error(inState, "expected Component");

                return nullptr;
            }

            return component;
        }
    }
}
