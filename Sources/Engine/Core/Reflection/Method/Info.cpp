#include "Chicane/Core/Reflection/Method/Info.hpp"

namespace Chicane
{
    ReflectionMethodInfo::ReflectionMethodInfo()
        : name(""),
          returnType(""),
          paramTypes({}),
          invoker({})
    {}

    void ReflectionMethodInfo::invoke(void* inInstance, Params inParams = {}) const
    {
        if (!invoker)
        {
            return;
        }

        invoker(inInstance, inParams);
    }
}