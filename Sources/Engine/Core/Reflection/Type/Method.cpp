#include "Chicane/Core/Reflection/Type/Method.hpp"

namespace Chicane
{
    ReflectionTypeMethod::ReflectionTypeMethod(const ReflectionTypeMethodInfo* inInstance)
        : m_instance(inInstance),
          m_params({})
    {}

    ReflectionTypeMethod::ReflectionTypeMethod()
        : m_instance(nullptr),
          m_params({})
    {}

    bool ReflectionTypeMethod::isValid() const
    {
        return m_instance != nullptr;
    }

    void ReflectionTypeMethod::invoke(void* inInstance) const
    {
        if (!isValid())
        {
            return;
        }

        m_instance->invoke(inInstance, m_params);
    }

    void ReflectionTypeMethod::addParam(Param inValue)
    {
        m_params.push_back(inValue);
    }
}