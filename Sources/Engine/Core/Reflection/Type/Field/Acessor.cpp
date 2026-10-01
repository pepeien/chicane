#include "Chicane/Core/Reflection/Type/Field/Acessor.hpp"

#include <cstdint>
#include <cstdio>

#include "Chicane/Core/FileSystem/Path.hpp"
#include "Chicane/Core/Math/Rotator.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Reflection/Enum/Registry.hpp"
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
        const void*                       inBoundInstance,
        bool                              bInIsTransient
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
          boundInstance(inBoundInstance),
          bIsTransient(bInIsTransient)
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
          boundInstance(nullptr),
          bIsTransient(false)
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
            if (const ReflectionTypeInfo* elementType = ReflectionTypeRegistry::sInstance().find(elementIndex.value()))
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
            element,
            false
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
            instance,
            bIsTransient
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
            for (std::size_t i = 0; i < count; i++)
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

        const ReflectionEnumInfo* enumeration = ReflectionEnumRegistry::sInstance().find(typeName);
        if (!enumeration)
        {
            const std::size_t split = typeName.lastOf(':');
            if (split != String::npos)
            {
                enumeration = ReflectionEnumRegistry::sInstance().find(typeName.substr(split + 1));
            }
        }

        if (enumeration)
        {
            const char* address = this->address(inInstance);
            int         value   = 0;
            if (address)
            {
                switch (size)
                {
                case 1:
                    value = static_cast<int>(*reinterpret_cast<const std::uint8_t*>(address));
                    break;

                case 2:
                    value = static_cast<int>(*reinterpret_cast<const std::uint16_t*>(address));
                    break;

                case 4:
                    value = *reinterpret_cast<const int*>(address);
                    break;

                default:
                    break;
                }
            }

            for (const ReflectionEnumeratorInfo& enumerator : enumeration->enumerators)
            {
                if (enumerator.value != value)
                {
                    continue;
                }

                const std::size_t split = enumerator.name.lastOf(':');
                if (split == String::npos)
                {
                    return enumerator.name;
                }

                return enumerator.name.substr(split + 1);
            }
        }

        if (const String* value = getValue<String>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const std::string* value = getValue<std::string>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const FileSystem::Path* value = getValue<FileSystem::Path>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const Vec2* value = getValue<Vec2>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const Vec3* value = getValue<Vec3>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const Vec4* value = getValue<Vec4>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const Rotator* value = getValue<Rotator>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const char* value = getValue<char>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const bool* value = getValue<bool>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const int* value = getValue<int>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const long* value = getValue<long>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const float* value = getValue<float>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const double* value = getValue<double>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const std::uint64_t* value = getValue<std::uint64_t>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const std::uint32_t* value = getValue<std::uint32_t>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const std::uint16_t* value = getValue<std::uint16_t>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (const std::uint8_t* value = getValue<std::uint8_t>(inInstance))
        {
            return static_cast<String>(*value);
        }

        if (elementIndex.has_value())
        {
            if (const ReflectionTypeInfo* elementType = ReflectionTypeRegistry::sInstance().find(elementIndex.value()))
            {
                if (elementType->findField(names.empty() ? String::sEmpty() : names.at(0)))
                {
                    return "<" + typeName + ">";
                }
            }
        }

        return "<" + typeName + ">";
    }

    const String& ReflectionFieldAccessor::getName() const
    {
        return names.empty() ? String::sEmpty() : names.at(0);
    }
}
