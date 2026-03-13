#include "Chicane/Grid/Method.hpp"

namespace Chicane
{
    namespace Grid
    {
        Method::Method()
            : m_instance(nullptr),
              m_params({})
        {}

        bool Method::isValid() const
        {
            return m_instance != nullptr;
        }

        void Method::invoke(void* inInstance) const
        {
            if (!isValid())
            {
                return;
            }

            m_instance->invoke(inInstance, m_params);
        }

        void Method::setInstance(const ReflectionMethodInfo* inValue)
        {
            m_instance = inValue;
        }

        void Method::addParam(Param inValue)
        {
            m_params.push_back(inValue);
        }
    }
}