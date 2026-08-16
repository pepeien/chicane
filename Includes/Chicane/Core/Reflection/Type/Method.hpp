#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Type/Method/Info.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionTypeMethod
    {
    public:
        using Param  = std::any;
        using Params = std::vector<Param>;

    public:
        ReflectionTypeMethod(const ReflectionTypeMethodInfo* inInstance);
        ReflectionTypeMethod();

    public:
        bool isValid() const;

        void addParam(Param inValue);

        std::any invoke(void* inInstance) const;

    private:
        const ReflectionTypeMethodInfo* m_instance;
        Params                          m_params;
    };
}