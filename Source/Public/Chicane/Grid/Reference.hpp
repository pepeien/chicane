#pragma once

#include <typeindex>

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct Reference
        {
        public:
            template<typename T>
            static Reference fromValue(T* inValue)
            {
                Reference result {};
                result.setValue<T>(inValue);

                return result;
            }

        public:
            bool isEmpty() const
            {
                return m_value == nullptr;
            }

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
                if (inValue == nullptr)
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
            const void*     m_value = nullptr;
            std::type_index m_type  = std::type_index(typeid(int));
        };
    }
}