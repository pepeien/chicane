#include "Chicane/Core/Script/Types.hpp"

#include <cstdint>

#include "Chicane/Core/FileSystem/Path.hpp"
#include "Chicane/Core/Reflection/Enum/Registry.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace Chicane
{
    namespace Script
    {
        namespace Types
        {
            static String typeTail(const String& inName)
            {
                const std::size_t split = inName.lastOf(':');
                if (split == String::npos)
                {
                    return inName;
                }

                return inName.substr(split + 1);
            }

            static String stripQualifiers(const String& inName)
            {
                String name = inName.trim();
                while (name.startsWith("const "))
                {
                    name = name.substr(6).trim();
                }

                while (name.endsWith("&") || name.endsWith("*"))
                {
                    name = name.substr(0, name.size() - 1).trim();
                }

                return name;
            }

            static bool typeMatches(const String& inName, const char* inExpected)
            {
                const String name = stripQualifiers(inName);
                return name.equals(inExpected) || typeTail(name).equals(inExpected);
            }

            static const ReflectionEnumInfo* findEnum(const String& inTypeName)
            {
                ReflectionEnumRegistry& registry = ReflectionEnumRegistry::sInstance();
                if (const ReflectionEnumInfo* found = registry.find(inTypeName))
                {
                    return found;
                }

                return registry.find(typeTail(inTypeName));
            }

            static void writeEnum(void* inAddress, std::size_t inSize, int inValue)
            {
                if (!inAddress)
                {
                    return;
                }

                switch (inSize)
                {
                case 1:
                    *static_cast<std::uint8_t*>(inAddress) = static_cast<std::uint8_t>(inValue);
                    break;

                case 2:
                    *static_cast<std::uint16_t*>(inAddress) = static_cast<std::uint16_t>(inValue);
                    break;

                case 4:
                    *static_cast<int*>(inAddress) = inValue;
                    break;

                default:
                    break;
                }
            }

            static bool setEnum(
                const ReflectionFieldAccessor& inAccessor, void* inInstance, lua_State* inState, int inIndex
            )
            {
                const ReflectionEnumInfo* info = findEnum(inAccessor.typeName);
                if (!info)
                {
                    return false;
                }

                int value = 0;
                if (lua_type(inState, inIndex) == LUA_TSTRING)
                {
                    const String name  = lua_tostring(inState, inIndex);
                    bool         found = false;
                    for (const ReflectionEnumeratorInfo& enumerator : info->enumerators)
                    {
                        if (!enumerator.name.equals(name) && !typeTail(enumerator.name).equals(name))
                        {
                            continue;
                        }

                        value = enumerator.value;
                        found = true;

                        break;
                    }

                    if (!found)
                    {
                        return false;
                    }
                }
                else if (lua_isnumber(inState, inIndex))
                {
                    value = static_cast<int>(lua_tointeger(inState, inIndex));
                }
                else
                {
                    return false;
                }

                writeEnum(inAccessor.address(inInstance), inAccessor.size, value);

                return true;
            }

            bool pushField(lua_State* inState, const ReflectionFieldAccessor& inAccessor, const void* inInstance)
            {
                if (!inAccessor.isValid() || inAccessor.bIsIterable || !inAccessor.address(inInstance))
                {
                    return false;
                }

                if (inAccessor.isType<bool>())
                {
                    lua_pushboolean(inState, inAccessor.get<bool>(inInstance) ? 1 : 0);

                    return true;
                }

                if (inAccessor.isType<float>())
                {
                    lua_pushnumber(inState, static_cast<lua_Number>(inAccessor.get<float>(inInstance)));

                    return true;
                }

                if (inAccessor.isType<double>())
                {
                    lua_pushnumber(inState, static_cast<lua_Number>(inAccessor.get<double>(inInstance)));

                    return true;
                }

                if (inAccessor.isType<int>())
                {
                    lua_pushinteger(inState, static_cast<lua_Integer>(inAccessor.get<int>(inInstance)));

                    return true;
                }

                if (inAccessor.isType<String>())
                {
                    lua_pushstring(inState, inAccessor.get<String>(inInstance).toChar());

                    return true;
                }

                if (inAccessor.isType<FileSystem::Path>())
                {
                    lua_pushstring(inState, inAccessor.get<FileSystem::Path>(inInstance).toString().toChar());

                    return true;
                }

                if (inAccessor.isType<Vec2>())
                {
                    pushVec2(inState, inAccessor.get<Vec2>(inInstance));

                    return true;
                }

                if (inAccessor.isType<Vec3>())
                {
                    pushVec3(inState, inAccessor.get<Vec3>(inInstance));

                    return true;
                }

                if (inAccessor.isType<Vec4>())
                {
                    pushVec4(inState, inAccessor.get<Vec4>(inInstance));

                    return true;
                }

                if (inAccessor.isType<Rotator>())
                {
                    pushRotator(inState, inAccessor.get<Rotator>(inInstance));

                    return true;
                }

                if (inAccessor.isType<Color::Rgba>())
                {
                    pushColor(inState, inAccessor.get<Color::Rgba>(inInstance));

                    return true;
                }

                if (findEnum(inAccessor.typeName))
                {
                    lua_pushstring(inState, inAccessor.toString(inInstance).toChar());

                    return true;
                }

                return false;
            }

            bool setField(lua_State* inState, const ReflectionFieldAccessor& inAccessor, void* inInstance, int inIndex)
            {
                if (!inAccessor.isValid() || inAccessor.bIsIterable || !inAccessor.address(inInstance))
                {
                    return false;
                }

                if (setEnum(inAccessor, inInstance, inState, inIndex))
                {
                    return true;
                }

                if (inAccessor.isType<bool>())
                {
                    inAccessor.set<bool>(inInstance, lua_toboolean(inState, inIndex) != 0);

                    return true;
                }

                if (inAccessor.isType<float>())
                {
                    inAccessor.set<float>(inInstance, static_cast<float>(luaL_checknumber(inState, inIndex)));

                    return true;
                }

                if (inAccessor.isType<double>())
                {
                    inAccessor.set<double>(inInstance, static_cast<double>(luaL_checknumber(inState, inIndex)));

                    return true;
                }

                if (inAccessor.isType<int>())
                {
                    inAccessor.set<int>(inInstance, static_cast<int>(luaL_checkinteger(inState, inIndex)));

                    return true;
                }

                if (inAccessor.isType<String>())
                {
                    inAccessor.set<String>(inInstance, luaL_checkstring(inState, inIndex));

                    return true;
                }

                if (inAccessor.isType<FileSystem::Path>())
                {
                    inAccessor.set<FileSystem::Path>(inInstance, FileSystem::Path(luaL_checkstring(inState, inIndex)));

                    return true;
                }

                if (inAccessor.isType<Vec2>())
                {
                    inAccessor.set<Vec2>(inInstance, checkVec2(inState, inIndex));

                    return true;
                }

                if (inAccessor.isType<Vec3>())
                {
                    inAccessor.set<Vec3>(inInstance, checkVec3(inState, inIndex));

                    return true;
                }

                if (inAccessor.isType<Vec4>())
                {
                    inAccessor.set<Vec4>(inInstance, checkVec4(inState, inIndex));

                    return true;
                }

                if (inAccessor.isType<Rotator>())
                {
                    inAccessor.set<Rotator>(inInstance, checkRotator(inState, inIndex));

                    return true;
                }

                if (inAccessor.isType<Color::Rgba>())
                {
                    inAccessor.set<Color::Rgba>(inInstance, checkColor(inState, inIndex));

                    return true;
                }

                return false;
            }

            int pushValue(lua_State* inState, const std::any& inValue)
            {
                if (!inValue.has_value())
                {
                    return 0;
                }

                if (const bool* value = std::any_cast<bool>(&inValue))
                {
                    lua_pushboolean(inState, *value ? 1 : 0);

                    return 1;
                }

                if (const float* value = std::any_cast<float>(&inValue))
                {
                    lua_pushnumber(inState, static_cast<lua_Number>(*value));

                    return 1;
                }

                if (const double* value = std::any_cast<double>(&inValue))
                {
                    lua_pushnumber(inState, static_cast<lua_Number>(*value));

                    return 1;
                }

                if (const int* value = std::any_cast<int>(&inValue))
                {
                    lua_pushinteger(inState, static_cast<lua_Integer>(*value));

                    return 1;
                }

                if (const String* value = std::any_cast<String>(&inValue))
                {
                    lua_pushstring(inState, value->toChar());

                    return 1;
                }

                if (const std::string* value = std::any_cast<std::string>(&inValue))
                {
                    lua_pushstring(inState, value->c_str());

                    return 1;
                }

                if (const FileSystem::Path* value = std::any_cast<FileSystem::Path>(&inValue))
                {
                    lua_pushstring(inState, value->toString().toChar());

                    return 1;
                }

                if (const Vec2* value = std::any_cast<Vec2>(&inValue))
                {
                    pushVec2(inState, *value);

                    return 1;
                }

                if (const Vec3* value = std::any_cast<Vec3>(&inValue))
                {
                    pushVec3(inState, *value);

                    return 1;
                }

                if (const Vec4* value = std::any_cast<Vec4>(&inValue))
                {
                    pushVec4(inState, *value);

                    return 1;
                }

                if (const Rotator* value = std::any_cast<Rotator>(&inValue))
                {
                    pushRotator(inState, *value);

                    return 1;
                }

                if (const Color::Rgba* value = std::any_cast<Color::Rgba>(&inValue))
                {
                    pushColor(inState, *value);

                    return 1;
                }

                return 0;
            }

            bool readValue(lua_State* inState, int inIndex, const String& inTypeName, std::any& outValue)
            {
                const String name = stripQualifiers(inTypeName);

                if (typeMatches(name, "bool"))
                {
                    outValue = lua_toboolean(inState, inIndex) != 0;

                    return true;
                }

                if (typeMatches(name, "float"))
                {
                    outValue = static_cast<float>(luaL_checknumber(inState, inIndex));

                    return true;
                }

                if (typeMatches(name, "double"))
                {
                    outValue = static_cast<double>(luaL_checknumber(inState, inIndex));

                    return true;
                }

                if (typeMatches(name, "int"))
                {
                    outValue = static_cast<int>(luaL_checkinteger(inState, inIndex));

                    return true;
                }

                if (typeMatches(name, "String") || typeMatches(name, "std::string"))
                {
                    outValue = String(luaL_checkstring(inState, inIndex));

                    return true;
                }

                if (typeMatches(name, "Path") || typeMatches(name, "FileSystem::Path"))
                {
                    outValue = FileSystem::Path(luaL_checkstring(inState, inIndex));

                    return true;
                }

                if (typeMatches(name, "Vec2"))
                {
                    outValue = checkVec2(inState, inIndex);

                    return true;
                }

                if (typeMatches(name, "Vec3"))
                {
                    outValue = checkVec3(inState, inIndex);

                    return true;
                }

                if (typeMatches(name, "Vec4"))
                {
                    outValue = checkVec4(inState, inIndex);

                    return true;
                }

                if (typeMatches(name, "Rotator"))
                {
                    outValue = checkRotator(inState, inIndex);

                    return true;
                }

                if (typeMatches(name, "Rgba") || typeMatches(name, "Color::Rgba"))
                {
                    outValue = checkColor(inState, inIndex);

                    return true;
                }

                return false;
            }
        }
    }
}
