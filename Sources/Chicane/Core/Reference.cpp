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

    std::string Reference::toString() const
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
}