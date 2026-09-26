#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem/Path.hpp"
#include "Chicane/Core/String.hpp"

struct lua_State;

namespace Chicane
{
    namespace Script
    {
        class CHICANE_CORE Context
        {
        public:
            static constexpr inline const char* FLAG_MODULE = "Chicane.flag";
            static constexpr inline const char* STWD_MODULE = "Chicane.stew";

        public:
            static Context* sFrom(lua_State* inState);

        public:
            Context();
            ~Context();

            Context(const Context&)            = delete;
            Context& operator=(const Context&) = delete;

            Context(Context&& inOther) noexcept;
            Context& operator=(Context&& inOther) noexcept;

        public:
            bool isOpen() const;
            lua_State* state() const;

            bool open(const char* inAllowedModule);
            void close();

            void setUser(void* inUser);
            void* getUser() const;

            bool loadFile(const FileSystem::Path& inPath);
            bool pcall(int inArgs = 0, int inResults = 0);

            bool pushGlobalFunction(const String& inName);
            bool callGlobal(const String& inName, int inArgs = 0);

            int ref();
            void unref(int inRef);
            bool callRef(int inRef, int inArgs = 0);

            void setHostModule();
            void reportError(const char* inPrefix = nullptr);

        private:
            void installSafeLibs();
            void installRequire();
            void denyUnsafeGlobals();
            void bindSelf();

        private:
            lua_State*  m_state;
            const char* m_allowed;
            void*       m_user;
        };
    }
}
