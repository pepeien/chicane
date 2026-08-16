#include "Chicane/Core/Reflection/Type/Field/Acessor.hpp"

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"

namespace Chicane
{
    ReflectionFieldAccessor::ReflectionFieldAccessor(
        std::size_t                       inOffset,
        std::size_t                       inPtrOffset,
        std::size_t                       inSize,
        const ReflectionFieldInfo::Names& inNames,
        const String&                     inTypeName,
        ReflectionFieldInfo::TypeIndex    inTypeIndex,
        bool                              bInNeedsDeref,
        bool                              bInIsIterable,
        ReflectionFieldInfo::TypeIndex    inElementIndex,
        ReflectionFieldIterable           inIterable,
        const void*                       inBoundInstance
    )
        : offset(inOffset),
          ptrOffset(inPtrOffset),
          size(inSize),
          names(std::move(inNames)),
          typeName(std::move(inTypeName)),
          typeIndex(inTypeIndex),
          elementIndex(inElementIndex),
          bNeedsDeref(bInNeedsDeref),
          bIsIterable(bInIsIterable),
          iterable(std::move(inIterable)),
          boundInstance(inBoundInstance)
    {}

    ReflectionFieldAccessor::ReflectionFieldAccessor()
        : offset(0),
          ptrOffset(0),
          size(0),
          names({}),
          typeName(""),
          typeIndex(std::nullopt),
          elementIndex(std::nullopt),
          bNeedsDeref(false),
          bIsIterable(false),
          iterable({}),
          boundInstance(nullptr)
    {}

    bool ReflectionFieldAccessor::isValid() const
    {
        if (boundInstance != nullptr)
        {
            return size > 0 && typeIndex.has_value();
        }

        return size > 0 && !typeName.isEmpty() && typeIndex.has_value();
    }

    const void* ReflectionFieldAccessor::containerPtr(const void* inInstance) const
    {
        if (boundInstance != nullptr)
        {
            return boundInstance;
        }

        return address(inInstance);
    }

    const char* ReflectionFieldAccessor::address(const void* inInstance) const
    {
        if (boundInstance != nullptr)
        {
            return static_cast<const char*>(boundInstance) + offset;
        }

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
        if (boundInstance != nullptr)
        {
            return const_cast<char*>(static_cast<const char*>(boundInstance) + offset);
        }

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

    std::size_t ReflectionFieldAccessor::getSize(const void* inInstance) const
    {
        if (!bIsIterable || !iterable.sizeFunction)
        {
            return 0;
        }

        const void* container = containerPtr(inInstance);
        if (!container)
        {
            return 0;
        }

        return iterable.sizeFunction(container);
    }

    ReflectionFieldAccessor ReflectionFieldAccessor::getElement(const void* inInstance, std::size_t inIndex) const
    {
        if (!bIsIterable || !iterable.atFunction)
        {
            return {};
        }

        const void* container = containerPtr(inInstance);
        if (!container)
        {
            return {};
        }

        const void* element = iterable.atFunction(container, inIndex);
        if (!element)
        {
            return {};
        }

        std::size_t elementSize = iterable.elementSize;
        if (elementSize == 0 && elementIndex.has_value())
        {
            if (const ReflectionTypeInfo* elementType =
                    ReflectionTypeRegistry::getInstance().find(elementIndex.value()))
            {
                elementSize = elementType->size;
            }
        }

        return {
            0,
            0,
            elementSize,
            {},
            iterable.elementTypeName,
            iterable.elementIndex,
            false,
            false,
            std::nullopt,
            {},
            element
        };
    }

    ReflectionFieldAccessor ReflectionFieldAccessor::bind(const void* inInstance) const
    {
        const void* instance = boundInstance != nullptr ? boundInstance : inInstance;

        return {
            offset,
            ptrOffset,
            size,
            names,
            typeName,
            typeIndex,
            bNeedsDeref,
            bIsIterable,
            elementIndex,
            iterable,
            instance
        };
    }

    String ReflectionFieldAccessor::toString(const void* inInstance) const
    {
        if (!isValid())
        {
            return "";
        }

        if (bIsIterable)
        {
            const std::size_t count = getSize(inInstance);

            String result = "[";
            for (std::size_t i = 0; i < count; ++i)
            {
                if (i > 0)
                {
                    result.append(", ");
                }

                result.append(getElement(inInstance, i).toString(inInstance));
            }

            result.append(']');

            return result;
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

        if (elementIndex.has_value())
        {
            if (const ReflectionTypeInfo* elementType =
                    ReflectionTypeRegistry::getInstance().find(elementIndex.value()))
            {
                if (elementType->findField(names.empty() ? String::empty() : names.at(0)))
                {
                    return "<" + typeName + ">";
                }
            }
        }

        return "<" + typeName + ">";
    }

    const String& ReflectionFieldAccessor::getName() const
    {
        return names.empty() ? String::empty() : names.at(0);
    }
}
