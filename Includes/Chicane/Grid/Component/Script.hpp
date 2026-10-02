#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Core/FileSystem/Path.hpp"
#include "Chicane/Core/Script/Context.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Script/Subscription.hpp"

struct lua_State;

namespace Chicane
{
    namespace Grid
    {
        class Component;
        class View;

        class CHICANE_GRID ComponentScript
        {
        public:
            static constexpr inline const char* EXTENSION = ".flag";

        public:
            explicit ComponentScript(Component* inHost);
            ~ComponentScript();

        public:
            bool load(const FileSystem::Path& inPath);
            void tick(float inDelta);
            bool callGlobal(const String& inName, const std::vector<String>& inArgs = {});

            void setOnLoad(int inRef);
            void setOnTick(int inRef);
            void pushFind(lua_State* inState, const char* inSelector);

            bool isBound() const;
            std::uint64_t subscribe(const String& inName, int inRef);
            void unsubscribe(std::uint64_t inToken);

            Component* host() const;
            View* view() const;
            Script::Context& context();

        private:
            void bind();
            void clearSubscriptions();

        private:
            Component*                          m_host;
            Script::Context                     m_context;
            int                                 m_onLoad;
            int                                 m_onTick;
            bool                                m_bClosing;
            std::vector<ComponentScriptSubscription> m_subscriptions;
        };
    }
}
