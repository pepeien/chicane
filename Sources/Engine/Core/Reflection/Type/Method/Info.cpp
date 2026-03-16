#include "Chicane/Core/Reflection/Type/Method/Info.hpp"

namespace Chicane
{
    ReflectionTypeMethodInfo::ReflectionTypeMethodInfo(String              inName,
                                                       String              inReturnType,
                                                       std::vector<String> inParamTypes,
                                                       Invoker             inInvoker)
        : name(std::move(inName)),
          returnType(std::move(inReturnType)),
          paramTypes(std::move(inParamTypes)),
          m_invoker(std::move(inInvoker))
    {}

    ReflectionTypeMethodInfo::ReflectionTypeMethodInfo()
        : name(""),
          returnType(""),
          paramTypes({}),
          m_invoker({})
    {}

    void ReflectionTypeMethodInfo::invoke(void* inInstance, Params inParams) const
    {
        if (!m_invoker)
        {
            return;
        }

        m_invoker(inInstance, inParams);
    }
}