#include "Chicane/Grid/Component/View/Script.hpp"

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Script/Channel.hpp"

#include "Chicane/Grid/Component/View.hpp"
#include "Chicane/Grid/Script/Types.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace Chicane
{
    namespace Grid
    {
        static ViewScript* hostFrom(lua_State* inState)
        {
            Script::Context* context = Script::Context::sFrom(inState);
            if (!context)
            {
                return nullptr;
            }

            return static_cast<ViewScript*>(context->getUser());
        }

        static int instanceOnLoad(lua_State* inState)
        {
            ViewScript* host = hostFrom(inState);
            luaL_checktype(inState, 1, LUA_TFUNCTION);
            if (!host)
            {
                return 0;
            }

            lua_pushvalue(inState, 1);
            host->setOnLoad(host->context().ref());

            return 0;
        }

        static int instanceOnTick(lua_State* inState)
        {
            ViewScript* host = hostFrom(inState);
            luaL_checktype(inState, 1, LUA_TFUNCTION);
            if (!host)
            {
                return 0;
            }

            lua_pushvalue(inState, 1);
            host->setOnTick(host->context().ref());

            return 0;
        }

        static int instanceFind(lua_State* inState)
        {
            ViewScript* host = hostFrom(inState);
            const char* raw  = luaL_checkstring(inState, 1);
            if (!host || !host->view())
            {
                lua_pushnil(inState);

                return 1;
            }

            host->pushFind(inState, raw);

            return 1;
        }

        static int instanceLog(lua_State* inState)
        {
            Log::emmit(Color::HEX_COLOR_LIME, "Flag", "%s", luaL_checkstring(inState, 1));

            return 0;
        }

        static int instanceSubscribe(lua_State* inState)
        {
            ViewScript* host = hostFrom(inState);
            const char* name = luaL_checkstring(inState, 1);
            luaL_checktype(inState, 2, LUA_TFUNCTION);
            if (!host || !host->view())
            {
                lua_pushinteger(inState, 0);

                return 1;
            }

            lua_pushvalue(inState, 2);
            const int ref = host->context().ref();

            const std::uint64_t token = host->view()->subscribe(
                name,
                [host, ref](const String& inData)
                {
                    if (!host->context().isOpen())
                    {
                        return;
                    }

                    lua_State* state = host->context().state();
                    lua_pushstring(state, inData.toChar());
                    host->context().callRef(ref, 1);
                }
            );

            lua_pushinteger(inState, static_cast<lua_Integer>(token));

            return 1;
        }

        static int instanceUnsubscribe(lua_State* inState)
        {
            ViewScript* host = hostFrom(inState);
            if (host && host->view())
            {
                host->view()->unsubscribe(static_cast<std::uint64_t>(luaL_checkinteger(inState, 1)));
            }

            return 0;
        }

        static int instanceSend(lua_State* inState)
        {
            ViewScript* host = hostFrom(inState);
            const char* name = luaL_checkstring(inState, 1);
            const char* data = luaL_optstring(inState, 2, "");
            if (host && host->view())
            {
                host->view()->send(name, data);
            }

            return 0;
        }

        static const luaL_Reg kInstanceMethods[] = {
            {"onLoad",      instanceOnLoad     },
            {"onTick",      instanceOnTick     },
            {"find",        instanceFind       },
            {"Log",         instanceLog        },
            {"subscribe",   instanceSubscribe  },
            {"unsubscribe", instanceUnsubscribe},
            {"send",        instanceSend       },
            {nullptr,       nullptr            }
        };

        ViewScript::ViewScript(View* inView)
            : m_view(inView),
              m_context(),
              m_onLoad(LUA_NOREF),
              m_onTick(LUA_NOREF)
        {}

        ViewScript::~ViewScript()
        {
            m_context.close();
        }

        bool ViewScript::load(const FileSystem::Path& inPath)
        {
            if (!m_context.open(Script::Context::FLAG_MODULE))
            {
                return false;
            }

            m_context.setUser(this);
            bind();

            if (!m_context.loadFile(inPath))
            {
                return false;
            }

            m_context.callRef(m_onLoad);

            return true;
        }

        void ViewScript::tick(float inDelta)
        {
            if (!m_context.isOpen() || m_onTick == LUA_NOREF)
            {
                return;
            }

            lua_pushnumber(m_context.state(), static_cast<lua_Number>(inDelta));
            m_context.callRef(m_onTick, 1);
        }

        bool ViewScript::callGlobal(const String& inName, const std::vector<String>& inArgs)
        {
            lua_State* state = m_context.state();
            if (!state || !m_context.pushGlobalFunction(inName))
            {
                return false;
            }

            for (const String& arg : inArgs)
            {
                lua_pushstring(state, arg.toChar());
            }

            return m_context.pcall(static_cast<int>(inArgs.size()), 0);
        }

        View* ViewScript::view() const
        {
            return m_view;
        }

        Script::Context& ViewScript::context()
        {
            return m_context;
        }

        void ViewScript::setOnLoad(int inRef)
        {
            m_context.unref(m_onLoad);
            m_onLoad = inRef;
        }

        void ViewScript::setOnTick(int inRef)
        {
            m_context.unref(m_onTick);
            m_onTick = inRef;
        }

        void ViewScript::pushFind(lua_State* inState, const char* inSelector)
        {
            if (!m_view || !inSelector)
            {
                lua_pushnil(inState);

                return;
            }

            const String            selector = inSelector;
            std::vector<Component*> tree;
            tree.push_back(m_view);
            m_view->appendChildrenFlat(tree);

            for (Component* component : tree)
            {
                if (!component)
                {
                    continue;
                }

                if (selector.startsWith("#"))
                {
                    if (component->getId().equals(selector.substr(1)))
                    {
                        Types::pushComponent(inState, component);

                        return;
                    }

                    continue;
                }

                String className = selector;
                if (className.startsWith("."))
                {
                    className = className.substr(1);
                }

                for (const String& part : component->getClassName().split(' '))
                {
                    if (part.equals(className))
                    {
                        Types::pushComponent(inState, component);

                        return;
                    }
                }
            }

            lua_pushnil(inState);
        }

        void ViewScript::bind()
        {
            lua_State* state = m_context.state();

            Types::bind(state);

            lua_newtable(state);
            luaL_setfuncs(state, kInstanceMethods, 0);
            lua_pushvalue(state, -1);
            lua_setglobal(state, "Instance");
            m_context.setHostModule();
            lua_pop(state, 1);
        }
    }
}
