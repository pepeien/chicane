#include "Chicane/Runtime/Application.hpp"

#include "Chicane/Core/Program/Option.hpp"

namespace Chicane
{
    inline const std::vector<String> EMPTY_ARG_VALUES = {};

    ApplicationArgs Application::sParseArgs(int inArgCount, char* inArgValues[])
    {
        ApplicationArgs args;

        for (int i = 1; i < inArgCount; ++i)
        {
            const String arg = inArgValues[i] ? inArgValues[i] : "";
            if (arg.isEmpty())
            {
                continue;
            }

            if (!arg.startsWith(ProgramOption::EXTENDED_PREFIX))
            {
                args[""].push_back(arg);

                continue;
            }

            String name = arg.substr(2);
            if (name.isEmpty())
            {
                continue;
            }

            if (name.contains("="))
            {
                const std::size_t equals = name.find("=");
                const String      key    = name.substr(0, equals);
                const String      value  = name.substr(equals + 1);
                if (!key.isEmpty())
                {
                    args[key].push_back(value);
                }

                continue;
            }

            std::vector<String>& values = args[name];
            while (i + 1 < inArgCount && inArgValues[i + 1] &&
                   !String(inArgValues[i + 1]).startsWith(ProgramOption::SHORT_PREFIX))
            {
                values.emplace_back(inArgValues[++i]);
            }
        }

        return args;
    }

    Application::Application(int inArgCount, char* inArgValues[])
        : m_args(sParseArgs(inArgCount, inArgValues))
    {}

    const ApplicationArgs& Application::getArgs() const
    {
        return m_args;
    }

    bool Application::hasArg(const String& inName) const
    {
        return m_args.find(inName) != m_args.end();
    }

    const std::vector<String>& Application::getArgValues(const String& inName) const
    {
        const auto found = m_args.find(inName);
        if (found == m_args.end())
        {
            return EMPTY_ARG_VALUES;
        }

        return found->second;
    }
}
