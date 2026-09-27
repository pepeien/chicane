#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Core/FileSystem/Path.hpp"
#include "Chicane/Core/Script/Context.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

struct lua_State;

namespace Chicane
{
    namespace Grid
    {
        class View;

        class CHICANE_GRID ViewScript
        {
        public:
            static constexpr inline const char* EXTENSION = ".flag";

        public:
            explicit ViewScript(View* inView);
            ~ViewScript();

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

            View* view() const;
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

            View*                     m_view;
            Script::Context           m_context;
            int                       m_onLoad;
            int                       m_onTick;
            bool                      m_bClosing;
            std::vector<Subscription> m_subscriptions;
        };
    }
}
