#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Application/Args.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME Application
    {
    private:
        static ApplicationArgs sParseArgs(int inArgCount, char* inArgValues[]);

    public:
        Application(int inArgCount, char* inArgValues[]);
        virtual ~Application() = default;

        Application(const Application&)            = delete;
        Application& operator=(const Application&) = delete;

        Application(Application&&)            = delete;
        Application& operator=(Application&&) = delete;

    public:
        const ApplicationArgs& getArgs() const;
        bool hasArg(const String& inName) const;
        const std::vector<String>& getArgValues(const String& inName) const;

    protected:
        ApplicationArgs m_args;
    };
}
