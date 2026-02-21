#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Program/Option.hpp"
#include "Chicane/Core/Program/Option/Setting.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    class CHICANE_CORE ProgramParam
    {
    public:
        using Flags       = std::vector<String>;
        using Positionals = std::vector<String>;

    public:
        ProgramParam();

    public:
        void addOption(const ProgramOptionSetting& inSetting);

        void parse(int inParamCount, char* inParamValue[]);

        bool hasFlag(const String& inValue) const;
        const Flags& getFlags() const;

        const Positionals& getPositionals() const;

        const ProgramOption::List& getOptions() const;
        const ProgramOption* getOption(const String& inName) const;

    private:
        ProgramOption* getOption(const String& inName);

    private:
        Flags               m_flags;
        Positionals         m_positionals;
        ProgramOption::List m_options;
    };
}