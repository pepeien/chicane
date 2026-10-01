#pragma once

#include <cstddef>
#include <new>
#include <typeinfo>

#include "Chicane/Core/Reflection/Type/Info.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Runtime/Script/Types.hpp"

struct lua_State;

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace Chicane
{
    namespace Types
    {
        static constexpr inline const char* OBJECT_MT   = "Chicane.Runtime.Object";
        static constexpr inline const char* OWNER_FIELD = "__chicane";

        struct ReflectedValueBox
        {
            const ReflectionTypeInfo* type;
            void (*destroy)(void*);
        };

        std::size_t reflectedValueDataOffset();
        void* reflectedValueData(ReflectedValueBox* inBox);
        void ensureValueMetatable(lua_State* inState, const char* inName, const ReflectionTypeInfo* inType);
        ReflectedValueBox* testValueBox(lua_State* inState, int inIndex, const char* inName);
        Object* liveObject(lua_State* inState, int inIndex);
        void bindReflectedStatics(lua_State* inState);

        int reflectedIndex(lua_State* inState);
        int reflectedNewIndex(lua_State* inState);

        inline const char* valueMetatableOf(const ReflectionTypeInfo* inType)
        {
            if (!inType || inType->getNames().empty())
            {
                return nullptr;
            }

            return inType->getNames().front().toChar();
        }

        template <typename T>
        const char* valueMetatableOf()
        {
            return valueMetatableOf(ReflectionTypeRegistry::sInstance().find(typeid(T)));
        }

        template <typename T>
        void pushValueCopy(lua_State* inState, const T& inValue)
        {
            const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(typeid(T));
            const char*               mt   = valueMetatableOf(type);
            if (!mt)
            {
                luaL_error(inState, "type is not reflected");

                return;
            }

            ensureValueMetatable(inState, mt, type);

            void*              memory = lua_newuserdatauv(inState, reflectedValueDataOffset() + sizeof(T), 0);
            ReflectedValueBox* box    = static_cast<ReflectedValueBox*>(memory);
            box->type                 = type;
            box->destroy              = [](void* inData) { static_cast<T*>(inData)->~T(); };
            new (reflectedValueData(box)) T(inValue);
            luaL_setmetatable(inState, mt);
        }

        template <typename T>
        bool isValueCopy(lua_State* inState, int inIndex)
        {
            const char* mt = valueMetatableOf<T>();

            return mt && testValueBox(inState, inIndex, mt) != nullptr;
        }

        template <typename T>
        T checkValueCopy(lua_State* inState, int inIndex)
        {
            const char* mt = valueMetatableOf<T>();
            if (!mt)
            {
                luaL_error(inState, "type is not reflected");

                return T();
            }

            ReflectedValueBox* box = testValueBox(inState, inIndex, mt);
            if (!box)
            {
                luaL_error(inState, "expected %s", mt);

                return T();
            }

            return *static_cast<const T*>(reflectedValueData(box));
        }
    }
}
