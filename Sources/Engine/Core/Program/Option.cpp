#include "Chicane/Core/Program/Option.hpp"

namespace Chicane
{
    bool ProgramOption::isValueValid() const
    {
        if (m_value.empty())
        {
            return !m_bIsRequired;
        }

        if (m_validValues.empty())
        {
            return true;
        }

        for (const String& value : m_value)
        {
            if (m_validValues.find(value) != m_validValues.end())
            {
                continue;
            }

            return false;
        }

        return true;
    }

    bool ProgramOption::isRequired() const
    {
        return m_bIsRequired;
    }

    void ProgramOption::setIsRequired(bool bInIsValue)
    {
        m_bIsRequired = bInIsValue;
    }

    bool ProgramOption::isName(const String& inValue) const
    {
        if (m_name.isEmpty() || inValue.isEmpty())
        {
            return false;
        }

        return m_name.equals(inValue) || m_name.at(0) == inValue;
    }

    const String& ProgramOption::getName() const
    {
        return m_name;
    }

    void ProgramOption::setName(const String& inValue)
    {
        m_name = inValue.trim();
    }

    const String& ProgramOption::getDescription() const
    {
        return m_description;
    }

    void ProgramOption::setDescription(const String& inValue)
    {
        m_description = inValue.trim();
    }

    const String& ProgramOption::getValue() const
    {
        return m_value;
    }

    void ProgramOption::setValue(const String& inValue)
    {
        if (inValue.isEmpty())
        {
            return;
        }

        m_value = inValue.trim();
    }

    const ProgramOption::Values& ProgramOption::getValidValues() const
    {
        return m_validValues;
    }

    void ProgramOption::addValidValue(const std::vector<String>& inValue)
    {
        if (inValue.empty())
        {
            return;
        }

        for (const String& value : inValue)
        {
            addValidValue(value);
        }
    }

    void ProgramOption::addValidValue(const String& inValue)
    {
        if (inValue.isEmpty())
        {
            return;
        }

        m_validValues.insert(inValue.trim());
    }

    bool ProgramOption::isValueValid(const String& inValue) const
    {
        if (m_validValues.empty())
        {
            return true;
        }

        return m_validValues.find(inValue) != m_validValues.end();
    }
}