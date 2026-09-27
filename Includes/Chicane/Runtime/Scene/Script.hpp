#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Core/FileSystem/Path.hpp"
#include "Chicane/Core/Script/Context.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Runtime.hpp"

struct lua_State;

namespace Chicane
{
    class Scene;

    class CHICANE_RUNTIME SceneScript
    {
    public:
        static constexpr inline const char* EXTENSION = ".stew";

    public:
        explicit SceneScript(Scene* inScene);
        ~SceneScript();

        bool load(const FileSystem::Path& inPath);
        void tick(float inDelta);
        bool callGlobal(const String& inName);

        void setOnLoad(int inRef);
        void setOnTick(int inRef);

        bool isBound() const;
        std::uint64_t subscribe(const String& inName, int inRef);
        void unsubscribe(std::uint64_t inToken);

        Scene* scene() const;
        Script::Context& context();

    private:
        void bind();
        void clearSubscriptions();

    private:
        struct Subscription
        {
            std::uint64_t token;
            int           ref;
        };

        Scene*                     m_scene;
        Script::Context            m_context;
        int                        m_onLoad;
        int                        m_onTick;
        bool                       m_bClosing;
        std::vector<Subscription>  m_subscriptions;
    };
}
