#include "Chicane/Grid/Component/View/Script.hpp"

#include <any>
#include <exception>

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Reflection/Type/Method.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Script/Channel.hpp"
#include "Chicane/Core/Script/Types.hpp"

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

        static int instanceView(lua_State* inState)
        {
            ViewScript* host = hostFrom(inState);
            if (!host || !host->view())
            {
                lua_pushnil(inState);

                return 1;
            }

            Types::pushComponent(inState, host->view());

            return 1;
        }

        static int instanceInvoke(lua_State* inState)
        {
            ViewScript* host = hostFrom(inState);
            const char* name = luaL_checkstring(inState, 1);
            if (!host || !host->host())
            {
                return 0;
            }

            Component*                      bound  = host->host();
            const ReflectionTypeInfo*       type   = ReflectionTypeRegistry::sInstance().find(typeid(*bound));
            const ReflectionTypeMethodInfo* method = type ? type->findMethod(name) : nullptr;

            if (!method && host->view() && host->view() != bound)
            {
                bound  = host->view();
                type   = ReflectionTypeRegistry::sInstance().find(typeid(*bound));
                method = type ? type->findMethod(name) : nullptr;
            }

            if (!method)
            {
                return luaL_error(inState, "unknown method '%s'", name);
            }

            ReflectionTypeMethod result(method);
            result.bind(bound);

            for (std::size_t i = 0; i < method->paramTypes.size(); i++)
            {
                std::any value;
                if (!Script::Types::readValue(inState, 2 + static_cast<int>(i), method->paramTypes[i], value))
                {
                    return luaL_error(
                        inState,
                        "invalid argument %d for %s",
                        static_cast<int>(i) + 1,
                        method->getName().toChar()
                    );
                }

                result.addParam(std::move(value));
            }

            try
            {
                return Script::Types::pushValue(inState, result.invoke());
            }
            catch (const std::exception& error)
            {
                return luaL_error(inState, "%s failed: %s", method->getName().toChar(), error.what());
            }
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

            lua_pushinteger(inState, static_cast<lua_Integer>(host->subscribe(name, ref)));

            return 1;
        }

        static int instanceUnsubscribe(lua_State* inState)
        {
            ViewScript* host = hostFrom(inState);
            if (host)
            {
                host->unsubscribe(static_cast<std::uint64_t>(luaL_checkinteger(inState, 1)));
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
            {"view",        instanceView       },
            {"invoke",      instanceInvoke     },
            {"Log",         instanceLog        },
            {"subscribe",   instanceSubscribe  },
            {"unsubscribe", instanceUnsubscribe},
            {"send",        instanceSend       },
            {nullptr,       nullptr            }
        };

        ViewScript::ViewScript(Component* inHost)
            : m_host(inHost),
              m_context(),
              m_onLoad(LUA_NOREF),
              m_onTick(LUA_NOREF),
              m_bClosing(false),
              m_subscriptions({})
        {}

        ViewScript::~ViewScript()
        {
            m_bClosing = true;
            clearSubscriptions();
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

        Component* ViewScript::host() const
        {
            return m_host;
        }

        View* ViewScript::view() const
        {
            if (!m_host)
            {
                return nullptr;
            }

            if (View* view = dynamic_cast<View*>(m_host))
            {
                return view;
            }

            return dynamic_cast<View*>(m_host->getRoot());
        }

        Script::Context& ViewScript::context()
        {
            return m_context;
        }

        bool ViewScript::isBound() const
        {
            return !m_bClosing && m_context.isOpen();
        }

        std::uint64_t ViewScript::subscribe(const String& inName, int inRef)
        {
            View* view = this->view();
            if (!view || !isBound())
            {
                m_context.unref(inRef);

                return 0;
            }

            const std::uint64_t token = view->subscribe(
                inName,
                [this, inRef](const String& inData)
                {
                    if (!isBound())
                    {
                        return;
                    }

                    lua_State* state = m_context.state();
                    lua_pushstring(state, inData.toChar());
                    m_context.callRef(inRef, 1);
                }
            );

            m_subscriptions.push_back({token, inRef});

            return token;
        }

        void ViewScript::unsubscribe(std::uint64_t inToken)
        {
            if (View* view = this->view())
            {
                view->unsubscribe(inToken);
            }

            for (auto it = m_subscriptions.begin(); it != m_subscriptions.end(); ++it)
            {
                if (it->token != inToken)
                {
                    continue;
                }

                m_context.unref(it->ref);
                m_subscriptions.erase(it);

                return;
            }
        }

        void ViewScript::clearSubscriptions()
        {
            for (const ViewScriptSubscription& subscription : m_subscriptions)
            {
                if (View* view = this->view())
                {
                    view->unsubscribe(subscription.token);
                }

                m_context.unref(subscription.ref);
            }

            m_subscriptions.clear();
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
            View* view = this->view();
            if (!view || !inSelector)
            {
                lua_pushnil(inState);

                return;
            }

            const String            selector = inSelector;
            std::vector<Component*> tree;
            tree.push_back(view);
            view->appendChildrenFlat(tree);

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

                if (component->classList.contains(className))
                {
                    Types::pushComponent(inState, component);

                    return;
                }
            }

            lua_pushnil(inState);
        }

        void ViewScript::bind()
        {
            lua_State* state = m_context.state();

            lua_newtable(state);
            luaL_setfuncs(state, kInstanceMethods, 0);
            lua_pushvalue(state, -1);
            lua_setglobal(state, "Instance");
            m_context.setHostModule();
            lua_pop(state, 1);
        }
    }
}
