#include "Chicane/Core/Reflection/Field/Acessor.hpp"

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

namespace Chicane
{
    ReflectionFieldAccessor::ReflectionFieldAccessor(
        std::size_t                       inOffset,
        std::size_t                       inPtrOffset,
        std::size_t                       inSize,
        const ReflectionFieldInfo::Names& inNames,
        const String&                     inTypeName,
        ReflectionFieldInfo::TypeIndex    inTypeIndex,
        bool                              bInNeedsDeref
    )
        : offset(inOffset),
          ptrOffset(inPtrOffset),
          size(inSize),
          names(std::move(inNames)),
          typeName(std::move(inTypeName)),
          typeIndex(inTypeIndex),
          bNeedsDeref(bInNeedsDeref)
    {}

    ReflectionFieldAccessor::ReflectionFieldAccessor()
        : offset(0),
          ptrOffset(0),
          size(0),
          names({}),
          typeName(""),
          typeIndex(std::nullopt),
          bNeedsDeref(false)
    {}

    bool ReflectionFieldAccessor::isValid() const
    {
        return size > 0 && !typeName.isEmpty() && typeIndex != std::nullopt;
    }

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
        if (!isValid())
        {
            return "";
        }

        if (isType<Vec2>())
        {
            const Vec2* v = getValue<Vec2>(inInstance);

            return v ? v->toString() : "";
        }

        if (isType<Vec3>())
        {
            const Vec3* v = getValue<Vec3>(inInstance);

            return v ? v->toString() : "";
        }

        if (isType<Vec4>())
        {
            const Vec4* v = getValue<Vec4>(inInstance);

            return v ? v->toString() : "";
        }

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

    const String& ReflectionFieldAccessor::getName() const
    {
        return names.empty() ? String::empty() : names.at(0);
    }
}