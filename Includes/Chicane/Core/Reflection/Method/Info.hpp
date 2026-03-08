#pragma once

#include <any>
#include <cstddef>
#include <functional>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionMethodInfo
    {
    public:
        using Params  = std::vector<std::any>;
        using Invoker = std::function<void(void*, Params)>;

    public:
        ReflectionMethodInfo(String inName, String inReturnType, std::vector<String> inParamTypes, Invoker inInvoker);
        ReflectionMethodInfo();

    public:
        void invoke(void* inInstance, Params inParams = {}) const;

    public:
        String              name;
        String              returnType;
        std::vector<String> paramTypes;

    private:
        Invoker m_invoker;
    };
}