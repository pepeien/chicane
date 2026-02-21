#include "Chicane/Core/Program/Param.hpp"

namespace Chicane
{
    ProgramParam::ProgramParam()
        : m_flags({}),
          m_positionals({}),
          m_options({})
    {}

    void ProgramParam::addOption(const ProgramOptionSetting& inSetting)
    {
        ProgramOption* option = getOption(inSetting.name);

        if (option == nullptr)
        {
            m_options.push_back({});

            option = &m_options.back();
        }

        option->setIsRequired(inSetting.bIsRequired);
        option->setName(inSetting.name);
        option->setDescription(inSetting.description);
        option->addValidValue(inSetting.validValues);
    }

    void ProgramParam::parse(int inParamCount, char* inParamValue[])
    {
        for (int i = 1; i < inParamCount; ++i)
        {
            String arg = inParamValue[i];

            if (!arg.startsWith(ProgramOption::SHORT_PREFIX))
            {
                m_positionals.push_back(arg);

                continue;
            }

            const bool bIsExtend = arg.startsWith(ProgramOption::EXTENDED_PREFIX);

            const String opt = arg.substr(bIsExtend ? 2 : 1);

            if (bIsExtend)
            {
                if (opt.contains("="))
                {
                    const std::size_t equalsLocation = opt.find("=");

                    const String name  = opt.substr(0, equalsLocation);
                    const String value = opt.substr(equalsLocation + 1);

                    if (ProgramOption* option = getOption(name))
                    {
                        option->addValue(value);
                    }

                    continue;
                }

                m_flags.push_back(opt);

                continue;
            }

            if (opt.size() >= 1)
            {
                if (opt.contains("="))
                {
                    const std::size_t equalsLocation = opt.find("=");

                    const String name  = opt.substr(0, equalsLocation);
                    const String value = opt.substr(equalsLocation + 1);

                    if (ProgramOption* option = getOption(opt))
                    {
                        option->addValue(value);
                    }

                    continue;
                }

                m_flags.push_back(opt);

                continue;
            }

            m_positionals.push_back(arg);
        }
    }

    bool ProgramParam::hasFlag(const String& inValue) const
    {
        return std::find_if(
                   m_flags.begin(),
                   m_flags.end(),
                   [&inValue](const String& inFlag) { return inFlag.equals(inValue); }
               ) != m_flags.end();
    }

    const ProgramParam::Flags& ProgramParam::getFlags() const
    {
        return m_flags;
    }

    const ProgramParam::Positionals& ProgramParam::getPositionals() const
    {
        return m_positionals;
    }

    const ProgramOption::List& ProgramParam::getOptions() const
    {
        return m_options;
    }

    const ProgramOption* ProgramParam::getOption(const String& inName) const
    {
        for (const ProgramOption& option : m_options)
        {
            if (!option.isName(inName))
            {
                continue;
            }

            return &option;
        }

        return nullptr;
    }

    ProgramOption* ProgramParam::getOption(const String& inName)
    {
        for (ProgramOption& option : m_options)
        {
            if (!option.isName(inName))
            {
                continue;
            }

            return &option;
        }

        return nullptr;
    }
}