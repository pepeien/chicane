#pragma once

#include <vector>
#include <unordered_set>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    class CHICANE_CORE ProgramOption
    {
    public:
        using List   = std::vector<ProgramOption>;
        using Values = std::unordered_set<String>;

    public:
        static constexpr inline const char  SHORT_PREFIX    = '-';
        static constexpr inline const char* EXTENDED_PREFIX = "--";

    public:
        bool isValueValid() const;

        bool isRequired() const;
        void setIsRequired(bool bInValue);

        bool isName(const String& inValue) const;
        const String& getName() const;
        void setName(const String& inValue);

        const String& getDescription() const;
        void setDescription(const String& inValue);

        const Values& getValues() const;
        void addValue(const String& inValue);

        const Values& getValidValues() const;
        void addValidValue(const std::vector<String>& inValue);
        void addValidValue(const String& inValue);

    private:
        bool isValueValid(const String& inValue) const;

    private:
        bool   m_bIsRequired = false;

        String m_name        = "";
        String m_description = "";

        Values m_values      = {};
        Values m_validValues = {};
    };
}