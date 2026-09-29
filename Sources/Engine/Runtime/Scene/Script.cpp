#include "Chicane/Runtime/Scene/Script.hpp"

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Script/Channel.hpp"
#include "Chicane/Core/Script/Types.hpp"

#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Component/View.hpp"
#include "Chicane/Runtime/Script/Types.hpp"

#include "../Script/Types/ReflectedValue.hpp"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace Chicane
{
    static SceneScript* hostFrom(lua_State* inState)
    {
        Script::Context* context = Script::Context::sFrom(inState);
        if (!context)
        {
            return nullptr;
        }

        return static_cast<SceneScript*>(context->getUser());
    }

    static std::vector<const Object*> readIgnored(lua_State* inState, int inIndex)
    {
        std::vector<const Object*> ignored;
        if (lua_isnoneornil(inState, inIndex))
        {
            return ignored;
        }

        luaL_checktype(inState, inIndex, LUA_TTABLE);

        const lua_Integer count = lua_rawlen(inState, inIndex);
        ignored.reserve(static_cast<std::size_t>(count));

        for (lua_Integer i = 1; i <= count; i++)
        {
            lua_rawgeti(inState, inIndex, i);
            ignored.push_back(Types::checkObject(inState, -1));
            lua_pop(inState, 1);
        }

        return ignored;
    }

    static bool readRequest(lua_State* inState, SceneTraceRequest& outRequest, int& outIgnoredIndex)
    {
        if (Types::isValueCopy<SceneTraceRequest>(inState, 1))
        {
            outRequest      = Types::checkValueCopy<SceneTraceRequest>(inState, 1);
            outIgnoredIndex = 2;

            return true;
        }

        if (Script::Types::isVec3(inState, 1) && Script::Types::isVec3(inState, 2))
        {
            outRequest =
                SceneTraceRequest::sLine(Script::Types::checkVec3(inState, 1), Script::Types::checkVec3(inState, 2));
            outIgnoredIndex = 3;

            return true;
        }

        if (Script::Types::isVec2(inState, 1))
        {
            if (!Script::Types::isVec4(inState, 2))
            {
                luaL_error(inState, "trace from a Vec2 expects a Vec4 viewport (left, top, right, bottom)");

                return false;
            }

            SceneScript* host      = hostFrom(inState);
            CView*       view      = dynamic_cast<CView*>(Types::checkComponent(inState, 3));
            const Vec4   rectangle = Script::Types::checkVec4(inState, 2);
            Bounds2D     viewport;
            viewport.set(rectangle.y, rectangle.x, rectangle.w, rectangle.z);

            if (!host || !host->scene() ||
                !host->scene()->trace(outRequest, Script::Types::checkVec2(inState, 1), viewport, view))
            {
                return false;
            }

            outIgnoredIndex = 4;

            return true;
        }

        luaL_error(
            inState,
            "trace expects a TraceRequest, origin and destination Vec3, or a Vec2 location with viewport and view"
        );

        return false;
    }

    static int instanceOnLoad(lua_State* inState)
    {
        SceneScript* host = hostFrom(inState);
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
        SceneScript* host = hostFrom(inState);
        luaL_checktype(inState, 1, LUA_TFUNCTION);
        if (!host)
        {
            return 0;
        }

        lua_pushvalue(inState, 1);
        host->setOnTick(host->context().ref());

        return 0;
    }

    static int instanceFindActor(lua_State* inState)
    {
        SceneScript* host = hostFrom(inState);
        const char*  id   = luaL_checkstring(inState, 1);
        if (!host || !host->scene())
        {
            lua_pushnil(inState);

            return 1;
        }

        Actor* actor = host->scene()->getActor(id);
        if (!actor)
        {
            lua_pushnil(inState);

            return 1;
        }

        Types::pushActor(inState, actor);

        return 1;
    }

    static int instanceGetActors(lua_State* inState)
    {
        SceneScript* host = hostFrom(inState);
        lua_newtable(inState);
        if (!host || !host->scene())
        {
            return 1;
        }

        int index = 1;
        for (Actor* actor : host->scene()->getActors())
        {
            if (!actor)
            {
                continue;
            }

            Types::pushActor(inState, actor);
            lua_rawseti(inState, -2, index++);
        }

        return 1;
    }

    static int instanceFindComponent(lua_State* inState)
    {
        SceneScript* host = hostFrom(inState);
        const char*  id   = luaL_checkstring(inState, 1);
        if (!host || !host->scene())
        {
            lua_pushnil(inState);

            return 1;
        }

        Component* component = host->scene()->getComponent(id);
        if (!component)
        {
            lua_pushnil(inState);

            return 1;
        }

        Types::pushComponent(inState, component);

        return 1;
    }

    static int instanceGetComponents(lua_State* inState)
    {
        SceneScript* host = hostFrom(inState);
        lua_newtable(inState);
        if (!host || !host->scene())
        {
            return 1;
        }

        int index = 1;
        for (Component* component : host->scene()->getComponents())
        {
            if (!component)
            {
                continue;
            }

            Types::pushComponent(inState, component);
            lua_rawseti(inState, -2, index++);
        }

        return 1;
    }

    static int instanceTrace(lua_State* inState)
    {
        SceneScript*      host = hostFrom(inState);
        SceneTraceRequest request;
        int               ignoredIndex = 2;
        if (!readRequest(inState, request, ignoredIndex))
        {
            lua_pushnil(inState);

            return 1;
        }

        if (!host || !host->scene())
        {
            lua_pushnil(inState);

            return 1;
        }

        SceneTraceResponse hit;
        if (!host->scene()->trace(hit, request, readIgnored(inState, ignoredIndex)))
        {
            lua_pushnil(inState);

            return 1;
        }

        Types::pushValueCopy(inState, hit);

        return 1;
    }

    static int instanceTraceMulti(lua_State* inState)
    {
        SceneScript*      host = hostFrom(inState);
        SceneTraceRequest request;
        int               ignoredIndex = 2;
        lua_newtable(inState);
        if (!readRequest(inState, request, ignoredIndex) || !host || !host->scene())
        {
            return 1;
        }

        std::vector<SceneTraceResponse> hits;
        host->scene()->traceMulti(hits, request, readIgnored(inState, ignoredIndex));

        int index = 1;
        for (const SceneTraceResponse& hit : hits)
        {
            Types::pushValueCopy(inState, hit);
            lua_rawseti(inState, -2, index++);
        }

        return 1;
    }

    static int instanceLog(lua_State* inState)
    {
        Log::emmit(Color::HEX_COLOR_LIME, "Steward", "%s", luaL_checkstring(inState, 1));

        return 0;
    }

    static int instanceSubscribe(lua_State* inState)
    {
        SceneScript* host = hostFrom(inState);
        const char*  name = luaL_checkstring(inState, 1);
        luaL_checktype(inState, 2, LUA_TFUNCTION);
        if (!host || !host->scene())
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
        SceneScript* host = hostFrom(inState);
        if (host)
        {
            host->unsubscribe(static_cast<std::uint64_t>(luaL_checkinteger(inState, 1)));
        }

        return 0;
    }

    static int instanceSend(lua_State* inState)
    {
        SceneScript* host = hostFrom(inState);
        const char*  name = luaL_checkstring(inState, 1);
        const char*  data = luaL_optstring(inState, 2, "");
        if (host && host->scene())
        {
            host->scene()->send(name, data);
        }

        return 0;
    }

    static const luaL_Reg kInstanceMethods[] = {
        {"onLoad",        instanceOnLoad       },
        {"onTick",        instanceOnTick       },
        {"findActor",     instanceFindActor    },
        {"getActors",     instanceGetActors    },
        {"findComponent", instanceFindComponent},
        {"getComponents", instanceGetComponents},
        {"trace",         instanceTrace        },
        {"traceMulti",    instanceTraceMulti   },
        {"Log",           instanceLog          },
        {"subscribe",     instanceSubscribe    },
        {"unsubscribe",   instanceUnsubscribe  },
        {"send",          instanceSend         },
        {nullptr,         nullptr              }
    };

    SceneScript::SceneScript(Scene* inScene)
        : m_scene(inScene),
          m_context(),
          m_onLoad(LUA_NOREF),
          m_onTick(LUA_NOREF),
          m_bClosing(false),
          m_subscriptions({})
    {}

    SceneScript::~SceneScript()
    {
        m_bClosing = true;
        clearSubscriptions();
        m_context.close();
    }

    bool SceneScript::load(const FileSystem::Path& inPath)
    {
        if (!m_context.open(Script::Context::STEWARD_MODULE))
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

    void SceneScript::tick(float inDelta)
    {
        if (!m_context.isOpen() || m_onTick == LUA_NOREF)
        {
            return;
        }

        lua_pushnumber(m_context.state(), static_cast<lua_Number>(inDelta));
        m_context.callRef(m_onTick, 1);
    }

    bool SceneScript::callGlobal(const String& inName)
    {
        return m_context.callGlobal(inName);
    }

    Scene* SceneScript::scene() const
    {
        return m_scene;
    }

    Script::Context& SceneScript::context()
    {
        return m_context;
    }

    bool SceneScript::isBound() const
    {
        return !m_bClosing && m_context.isOpen();
    }

    std::uint64_t SceneScript::subscribe(const String& inName, int inRef)
    {
        if (!m_scene || !isBound())
        {
            m_context.unref(inRef);

            return 0;
        }

        const std::uint64_t token = m_scene->subscribe(
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

    void SceneScript::unsubscribe(std::uint64_t inToken)
    {
        if (m_scene)
        {
            m_scene->unsubscribe(inToken);
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

    void SceneScript::clearSubscriptions()
    {
        for (const SceneScriptSubscription& subscription : m_subscriptions)
        {
            if (m_scene)
            {
                m_scene->unsubscribe(subscription.token);
            }

            m_context.unref(subscription.ref);
        }

        m_subscriptions.clear();
    }

    void SceneScript::setOnLoad(int inRef)
    {
        m_context.unref(m_onLoad);
        m_onLoad = inRef;
    }

    void SceneScript::setOnTick(int inRef)
    {
        m_context.unref(m_onTick);
        m_onTick = inRef;
    }

    void SceneScript::bind()
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
