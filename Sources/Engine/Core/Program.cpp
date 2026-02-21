#include "Chicane/Core/Program.hpp"

#include <iostream>
#include <iomanip>

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

        std::size_t maxNameSize        = 0U;
        std::size_t maxDescriptionSize = 0U;
        for (const ProgramOption& option : m_param.getOptions())
        {
            if (!option.getName().isEmpty())
            {
                std::size_t nameSize = option.getName().size();
                if (nameSize > maxNameSize)
                {
                    maxNameSize = nameSize;
                }
            }

            if (!option.getDescription().isEmpty())
            {
                std::size_t descriptionSize = option.getDescription().size();
                if (descriptionSize > maxDescriptionSize)
                {
                    maxDescriptionSize = descriptionSize;
                }
            }
        }

        for (const ProgramOption& option : m_param.getOptions())
        {
            const String& name        = option.getName();
            const String& description = option.getDescription();

            String validValues = "";
            if (!option.getValidValues().empty())
            {
                for (const String& validValue : option.getValidValues())
                {
                    validValues.append(validValue);
                    validValues.append(',');
                }
                validValues.popBack();
            }

            std::cout << std::left;

            if (!description.isEmpty())
            {
                std::cout << "\t-" << name.at(0) << " --" << name;
                std::cout << std::setw(maxNameSize - name.size()) << " ";

                if (name.size() < maxNameSize)
                {
                    std::cout << " ";
                }
            }

            if (!description.isEmpty())
            {
                std::cout << "\"" << description << "\"";
            }

            if (!validValues.isEmpty())
            {
                std::cout << " ";
                std::cout << std::setw(maxDescriptionSize - description.size()) << " ";
                std::cout << "[ " << validValues << " ]";
            }

            std::cout << std::endl;
        }
    }
}