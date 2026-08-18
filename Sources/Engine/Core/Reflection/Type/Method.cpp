#include "Chicane/Core/Reflection/Type/Method.hpp"

namespace Chicane
{
    ReflectionTypeMethod::ReflectionTypeMethod(const ReflectionTypeMethodInfo* inInstance)
        : m_instance(inInstance),
          m_boundInstance(nullptr),
          m_params({})
    {}

    ReflectionTypeMethod::ReflectionTypeMethod()
        : m_instance(nullptr),
          m_boundInstance(nullptr),
          m_params({})
    {}

    bool ReflectionTypeMethod::isValid() const
    {
        return m_instance != nullptr;
    }

    void ReflectionTypeMethod::bind(void* inInstance)
    {
        m_boundInstance = inInstance;
    }

    void* ReflectionTypeMethod::getBoundInstance() const
    {
        return m_boundInstance;
    }

    std::any ReflectionTypeMethod::invoke() const
    {
        return invoke(m_boundInstance);
    }

    std::any ReflectionTypeMethod::invoke(void* inInstance) const
    {
        if (!isValid())
        {
            return {};
        }

        return m_instance->invoke(inInstance != nullptr ? inInstance : m_boundInstance, m_params);
    }

    void ReflectionTypeMethod::addParam(Param inValue)
    {
        m_params.push_back(inValue);
    }
}