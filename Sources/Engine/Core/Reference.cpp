#include "Chicane/Core/Reference.hpp"

namespace Chicane
{
    Reference::Reference()
        : m_value(nullptr),
        m_type(std::type_index(typeid(int)))
    {}

    bool Reference::isEmpty() const
    {
        return m_value == nullptr;
    }

    String Reference::toString() const
    {
        // String
        if (isType<String>())
        {
            return *getValue<String>();
        }

        if (isType<char>())
        {
            return *getValue<char>();
        }

        // Primitive
        if (isType<bool>())
        {
            return std::to_string(*getValue<bool>());
        }

        if (isType<int>())
        {
            return std::to_string(*getValue<int>());
        }

        if (isType<long>())
        {
            return std::to_string(*getValue<long>());
        }

        if (isType<float>())
        {
            return std::to_string(*getValue<float>());
        }

        // Unsigned Integer
        if (isType<std::uint64_t>())
        {
            return std::to_string(*getValue<std::uint64_t>());
        }

        if (isType<std::uint32_t>())
        {
            return std::to_string(*getValue<std::uint32_t>());
        }

        if (isType<std::uint16_t>())
        {
            return std::to_string(*getValue<std::uint16_t>());
        }

        if (isType<std::uint8_t>())
        {
            return std::to_string(*getValue<std::uint8_t>());
        }

        return "";
    }
}