#include "Chicane/Core/Program.hpp"

#include <iostream>

namespace Chicane
{
    Program::Program(const String& inName)
        : m_name(inName.trim()),
          m_param({})
    {}

    Program& Program::addOption(const ProgramOptionSetting& inSetting)
    {
        m_param.addOption(inSetting);

        return *this;
    }

    void Program::exec(int inParamCount, char* inParamValue[])
    {
        m_param.parse(inParamCount, inParamValue);

        if (m_param.hasFlag('h') || m_param.hasFlag("help"))
        {
            showHelp();

            return;
        }

        for (const ProgramOption& option : m_param.getOptions())
        {
            if (option.isValueValid())
            {
                continue;
            }

            std::cout << "Option [ " << option.getName() << " ] is invalid." << std::endl;

            return;
        }

        onExec(m_param);
    }

    void Program::showHelp() const
    {
        std::cout << "Usage: " << m_name << " [options] [--] [args]" << std::endl;
        std::cout << "Options:" << std::endl;

        std::size_t maxDescriptionSize = 0U;
        for (const ProgramOption& option : m_param.getOptions())
        {
            if (option.getName().isEmpty())
            {
                continue;
            }

            std::size_t descriptionSize = option.getDescription().size();

            if (descriptionSize <= maxDescriptionSize)
            {
                continue;
            }

            maxDescriptionSize = option.getDescription().size();
        }

        for (const ProgramOption& option : m_param.getOptions())
        {
            if (option.getName().isEmpty())
            {
                continue;
            }

            std::cout << "\t-" << option.getName().at(0) << " --" << option.getName();

            if (option.getDescription().size() >= maxDescriptionSize)
            {
                std::cout << " ";
            }

            for (std::size_t i = option.getDescription().size(); i < maxDescriptionSize; i++)
            {
                std::cout << " ";
            }

            std::cout << " \"" << option.getDescription() << "\"";

            if (!option.getValidValues().empty())
            {
                String validValues = "";

                for (const String& validValue : option.getValidValues())
                {
                    validValues.append(validValue);
                    validValues.append(',');
                }
                validValues.popBack();

                std::cout << " [ " << validValues.toStandard() << " ] ";
            }

            std::cout << std::endl;
        }
    }
}