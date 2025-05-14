#pragma once

#include <typeindex>

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE Reference
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

            std::string toString() const
            {
                if (isType<std::string>())
                {
                    return *getValue<std::string>();
                }

                if (isType<bool>())
                {
                    return std::to_string(*getValue<bool>());
                }

                if (isType<int>())
                {
                    return std::to_string(*getValue<int>());
                }

                if (isType<std::uint64_t>())
                {
                    return std::to_string(*getValue<std::uint64_t>());
                }

                if (isType<std::uint32_t>())
                {
                    return std::to_string(*getValue<std::uint32_t>());
                }

                if (isType<float>())
                {
                    return std::to_string(*getValue<float>());
                }

                return "";
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
            const void*     m_value = nullptr;
            std::type_index m_type  = std::type_index(typeid(int));
        };

        typedef std::unordered_map<std::string, Reference*> References;
    }
}