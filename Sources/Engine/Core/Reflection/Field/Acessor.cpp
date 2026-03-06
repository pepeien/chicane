#include "Chicane/Core/Reflection/Field/Acessor.hpp"

namespace Chicane
{
    const char* ReflectionFieldAccessor::address(const void* inInstance) const
    {
        const char* base = static_cast<const char*>(inInstance) + offset;

        if (bNeedsDeref)
        {
            const void* pointee = *reinterpret_cast<const void* const*>(base);
            if (!pointee)
            {
                return nullptr;
            }

            return static_cast<const char*>(pointee) + ptrOffset;
        }

        return base;
    }

    char* ReflectionFieldAccessor::address(void* inInstance) const
    {
        char* base = static_cast<char*>(inInstance) + offset;

        if (bNeedsDeref)
        {
            void* pointee = *reinterpret_cast<void**>(base);
            if (!pointee)
            {
                return nullptr;
            }
            return static_cast<char*>(pointee) + ptrOffset;
        }

        return base;
    }

    void* ReflectionFieldAccessor::ptr(void* inInstance) const
    {
        return static_cast<char*>(inInstance) + offset;
    }

    const void* ReflectionFieldAccessor::ptr(const void* inInstance) const
    {
        return static_cast<const char*>(inInstance) + offset;
    }

    String ReflectionFieldAccessor::toString(const void* inInstance) const
    {
        if (isType<String>())
        {
            const String* v = getValue<String>(inInstance);

            return v ? *v : "";
        }

        if (isType<std::string>())
        {
            const std::string* v = getValue<std::string>(inInstance);

            return v ? *v : "";
        }

        if (isType<char>())
        {
            const char* v = getValue<char>(inInstance);

            return v ? String(1, *v) : "";
        }

        if (isType<bool>())
        {
            const bool* v = getValue<bool>(inInstance);

            return v ? (*v ? "true" : "false") : "";
        }

        if (isType<int>())
        {
            const int* v = getValue<int>(inInstance);

            return v ? std::to_string(*v) : "";
        }

        if (isType<long>())
        {
            const long* v = getValue<long>(inInstance);

            return v ? std::to_string(*v) : "";
        }

        if (isType<float>())
        {
            const float* v = getValue<float>(inInstance);

            return v ? std::to_string(*v) : "";
        }

        if (isType<double>())
        {
            const double* v = getValue<double>(inInstance);

            return v ? std::to_string(*v) : "";
        }

        if (isType<std::uint64_t>())
        {
            const std::uint64_t* v = getValue<std::uint64_t>(inInstance);

            return v ? std::to_string(*v) : "";
        }

        if (isType<std::uint32_t>())
        {
            const std::uint32_t* v = getValue<std::uint32_t>(inInstance);

            return v ? std::to_string(*v) : "";
        }

        if (isType<std::uint16_t>())
        {
            const std::uint16_t* v = getValue<std::uint16_t>(inInstance);

            return v ? std::to_string(*v) : "";
        }

        if (isType<std::uint8_t>())
        {
            const std::uint8_t* v = getValue<std::uint8_t>(inInstance);

            return v ? std::to_string(*v) : "";
        }

        return "<" + typeName + ">";
    }
}