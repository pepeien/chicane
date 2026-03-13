#include "Chicane/Core/Reflection/Method/Info.hpp"

namespace Chicane
{
    ReflectionMethodInfo::ReflectionMethodInfo(
        String inName, String inReturnType, std::vector<String> inParamTypes, Invoker inInvoker
    )
        : name(std::move(inName)),
          returnType(std::move(inReturnType)),
          paramTypes(std::move(inParamTypes)),
          m_invoker(std::move(inInvoker))
    {}

    ReflectionMethodInfo::ReflectionMethodInfo()
        : name(""),
          returnType(""),
          paramTypes({}),
          m_invoker({})
    {}

    void ReflectionMethodInfo::invoke(void* inInstance, Params inParams) const
    {
        if (!m_invoker)
        {
            return;
        }

        m_invoker(inInstance, inParams);
    }
}