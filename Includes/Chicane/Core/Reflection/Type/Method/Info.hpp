#pragma once

#include <any>
#include <cstddef>
#include <functional>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionTypeMethodInfo
    {
    public:
        using Params  = std::vector<std::any>;
        using Invoker = std::function<std::any(void*, Params)>;

    public:
        ReflectionTypeMethodInfo(
            String inName, String inReturnType, std::vector<String> inParamTypes, Invoker inInvoker
        );
        ReflectionTypeMethodInfo();

    public:
        std::any invoke(void* inInstance, Params inParams = {}) const;
        String toString(const std::any& inValue) const;

    public:
        String              name;
        String              returnType;
        std::vector<String> paramTypes;

    private:
        Invoker m_invoker;
    };
}