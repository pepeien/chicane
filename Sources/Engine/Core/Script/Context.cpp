#include "Chicane/Core/Script/Context.hpp"

#include <cstring>

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Script/Types.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

namespace Chicane
{
    namespace Script
    {
        static constexpr inline const char* HOST_REGISTRY_KEY    = "Chicane.Script.Host";
        static constexpr inline const char* ALLOWED_REGISTRY_KEY = "Chicane.Script.Allowed";

        static int denied(lua_State* inState)
        {
            return luaL_error(inState, "this function is not available");
        }

        static int requireHost(lua_State* inState)
        {
            const char* name = luaL_checkstring(inState, 1);
            lua_getfield(inState, LUA_REGISTRYINDEX, ALLOWED_REGISTRY_KEY);
            const char* allowed = lua_tostring(inState, -1);
            lua_pop(inState, 1);

            if (!allowed || std::strcmp(name, allowed) != 0)
            {
                return luaL_error(
                    inState,
                    "cannot require '%s'%s%s%s",
                    name,
                    allowed ? " (only '" : "",
                    allowed ? allowed : "",
                    allowed ? "' is available)" : ""
                );
            }

            lua_getfield(inState, LUA_REGISTRYINDEX, HOST_REGISTRY_KEY);
            return 1;
        }

        Context* Context::sFrom(lua_State* inState)
        {
            if (!inState)
            {
                return nullptr;
            }

            return *static_cast<Context**>(lua_getextraspace(inState));
        }

        Context::Context()
            : m_state(nullptr),
              m_allowed(nullptr),
              m_user(nullptr)
        {}

        Context::~Context()
        {
            close();
        }

        Context::Context(Context&& inOther) noexcept
            : m_state(inOther.m_state),
              m_allowed(inOther.m_allowed),
              m_user(inOther.m_user)
        {
            inOther.m_state   = nullptr;
            inOther.m_allowed = nullptr;
            inOther.m_user    = nullptr;

            bindSelf();
        }

        Context& Context::operator=(Context&& inOther) noexcept
        {
            if (this == &inOther)
            {
                return *this;
            }

            close();

            m_state   = inOther.m_state;
            m_allowed = inOther.m_allowed;
            m_user    = inOther.m_user;

            inOther.m_state   = nullptr;
            inOther.m_allowed = nullptr;
            inOther.m_user    = nullptr;

            bindSelf();

            return *this;
        }

        bool Context::isOpen() const
        {
            return m_state != nullptr;
        }

        lua_State* Context::state() const
        {
            return m_state;
        }

        bool Context::open(const char* inAllowedModule)
        {
            close();

            m_state = luaL_newstate();
            if (!m_state)
            {
                Log::error("Failed to create Lua state");

                return false;
            }

            m_allowed = inAllowedModule;
            bindSelf();
            installSafeLibs();
            Types::bind(m_state);
            denyUnsafeGlobals();
            installRequire();

            return true;
        }

        void Context::close()
        {
            if (!m_state)
            {
                return;
            }

            lua_close(m_state);
            m_state   = nullptr;
            m_allowed = nullptr;
        }

        void Context::setUser(void* inUser)
        {
            m_user = inUser;
        }

        void* Context::getUser() const
        {
            return m_user;
        }

        bool Context::loadFile(const FileSystem::Path& inPath)
        {
            if (!m_state || inPath.isEmpty())
            {
                return false;
            }

            const String source = inPath.toString();
            if (luaL_loadfile(m_state, source.toChar()) != LUA_OK)
            {
                reportError("Lua load");

                return false;
            }

            return pcall();
        }

        bool Context::pcall(int inArgs, int inResults)
        {
            if (!m_state)
            {
                return false;
            }

            const int error = lua_pcall(m_state, inArgs, inResults, 0);
            if (error != LUA_OK)
            {
                reportError("Lua");

                return false;
            }

            return true;
        }

        bool Context::pushGlobalFunction(const String& inName)
        {
            if (!m_state || inName.isEmpty())
            {
                return false;
            }

            lua_getglobal(m_state, inName.toChar());
            if (!lua_isfunction(m_state, -1))
            {
                lua_pop(m_state, 1);

                return false;
            }

            return true;
        }

        bool Context::callGlobal(const String& inName, int inArgs)
        {
            if (!pushGlobalFunction(inName))
            {
                if (inArgs > 0 && m_state)
                {
                    lua_pop(m_state, inArgs);
                }

                return false;
            }

            if (inArgs > 0)
            {
                lua_insert(m_state, -(inArgs + 1));
            }

            return pcall(inArgs, 0);
        }

        int Context::ref()
        {
            if (!m_state)
            {
                return LUA_NOREF;
            }

            return luaL_ref(m_state, LUA_REGISTRYINDEX);
        }

        void Context::unref(int inRef)
        {
            if (!m_state || inRef == LUA_NOREF || inRef == LUA_REFNIL)
            {
                return;
            }

            luaL_unref(m_state, LUA_REGISTRYINDEX, inRef);
        }

        bool Context::callRef(int inRef, int inArgs)
        {
            if (!m_state || inRef == LUA_NOREF || inRef == LUA_REFNIL)
            {
                if (inArgs > 0 && m_state)
                {
                    lua_pop(m_state, inArgs);
                }

                return false;
            }

            lua_rawgeti(m_state, LUA_REGISTRYINDEX, inRef);
            if (!lua_isfunction(m_state, -1))
            {
                lua_pop(m_state, 1 + inArgs);

                return false;
            }

            if (inArgs > 0)
            {
                lua_insert(m_state, -(inArgs + 1));
            }

            return pcall(inArgs, 0);
        }

        void Context::setHostModule()
        {
            if (!m_state)
            {
                return;
            }

            lua_pushvalue(m_state, -1);
            lua_setfield(m_state, LUA_REGISTRYINDEX, HOST_REGISTRY_KEY);
        }

        void Context::reportError(const char* inPrefix)
        {
            if (!m_state)
            {
                return;
            }

            const char* message = lua_tostring(m_state, -1);
            if (inPrefix && inPrefix[0] != '\0')
            {
                Log::error("%s: %s", inPrefix, message ? message : "unknown error");
            }
            else
            {
                Log::error("%s", message ? message : "unknown Lua error");
            }

            lua_pop(m_state, 1);
        }

        void Context::installSafeLibs()
        {
            const int safe = LUA_GLIBK | LUA_COLIBK | LUA_STRLIBK | LUA_TABLIBK | LUA_MATHLIBK | LUA_UTF8LIBK;
            luaL_openselectedlibs(m_state, safe, 0);
        }

        void Context::installRequire()
        {
            if (m_allowed)
            {
                lua_pushstring(m_state, m_allowed);
                lua_setfield(m_state, LUA_REGISTRYINDEX, ALLOWED_REGISTRY_KEY);
            }

            lua_pushcfunction(m_state, requireHost);
            lua_setglobal(m_state, "require");
        }

        void Context::denyUnsafeGlobals()
        {
            const char* denied[] = {"load", "loadfile", "dofile", "loadstring", nullptr};
            for (int i = 0; denied[i]; ++i)
            {
                lua_pushcfunction(m_state, ::Chicane::Script::denied);
                lua_setglobal(m_state, denied[i]);
            }
        }

        void Context::bindSelf()
        {
            if (!m_state)
            {
                return;
            }

            *static_cast<Context**>(lua_getextraspace(m_state)) = this;
        }
    }
}
