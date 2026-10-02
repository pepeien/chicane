#pragma once

#include <algorithm>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionProperty
    {
    public:
        using Names = std::vector<String>;

    public:
        ReflectionProperty(
            Names inNames = {}, String inGroup = {}, String inDescription = {}, bool bInIsTransient = false
        );

    public:
        bool containsName(const String& inValue) const;
        const String& getName() const;

        bool isTransient() const;

    public:
        Names  names;
        String group;
        String description;
        bool   bIsTransient;
    };
}
