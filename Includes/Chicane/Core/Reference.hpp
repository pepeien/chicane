#pragma once

#include <typeindex>

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    struct CHICANE_CORE Reference
    {
    public:
        template<typename T>
        static Reference fromValue(T* inValue)
        {
            Reference result = Reference();
            result.setValue<T>(inValue);

            return result;
        }

    public:
        Reference();
        ~Reference() = default;

    public:
        bool isEmpty() const;

        std::string toString() const;

        template<typename T>
        bool isType() const
        {
            if (isEmpty())
            {
                return false;
            }

            return m_type == std::type_index(typeid(T));
        }

        template<typename T>
        void setValue(T* inValue)
        {
            if (!inValue)
            {
                return;
            }

            m_value = inValue;
            m_type  = std::type_index(typeid(T));
        }

        template<typename T>
        const T* getValue() const
        {
            if (!isType<T>())
            {
                return nullptr;
            }

            return static_cast<const T*>(m_value);
        }
    
    private:
        const void*     m_value;
        std::type_index m_type;
    };

    typedef std::unordered_map<std::string, Reference*> References;
}