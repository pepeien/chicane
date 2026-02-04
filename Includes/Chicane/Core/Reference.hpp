#pragma once

#include <typeindex>
#include <unordered_map>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE Reference
    {
    public:
        using Map = std::unordered_map<String, Reference*>;

    public:
        template <typename T>
        static inline Reference fromValue(T* inValue)
        {
            Reference result = Reference();
            result.setValue<T>(inValue);

            return result;
        }

    public:
        Reference();

    public:
        bool isEmpty() const;

        String toString() const;

        template <typename T>
        inline bool isType() const
        {
            if (isEmpty())
            {
                return false;
            }

            return m_type == std::type_index(typeid(T));
        }

        template <typename T>
        inline void setValue(T* inValue)
        {
            if (!inValue)
            {
                return;
            }

            m_value = inValue;
            m_type  = std::type_index(typeid(T));
        }

        template <typename T>
        inline const T* getValue() const
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
}