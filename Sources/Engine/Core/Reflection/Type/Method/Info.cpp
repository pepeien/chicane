#include "Chicane/Core/Reflection/Type/Method/Info.hpp"

#include "Chicane/Core/FileSystem/Path.hpp"
#include "Chicane/Core/Reflection/Type/Info.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"

namespace Chicane
{
    ReflectionTypeMethodInfo::ReflectionTypeMethodInfo(
        ReflectionProperty      inProperty,
        String                  inReturnType,
        std::vector<String>     inParamTypes,
        Invoker                 inInvoker,
        bool                    bInIsIterable,
        TypeIndex               inReturnTypeIndex,
        TypeIndex               inElementIndex,
        std::size_t             inReturnSize,
        ReflectionFieldIterable inIterable,
        ContainerResolver       inContainerResolver,
        bool                    bInIsStatic
    )
        : property(std::move(inProperty)),
          returnType(std::move(inReturnType)),
          paramTypes(std::move(inParamTypes)),
          bIsIterable(bInIsIterable),
          bIsStatic(bInIsStatic),
          returnTypeIndex(std::move(inReturnTypeIndex)),
          elementIndex(std::move(inElementIndex)),
          returnSize(inReturnSize),
          iterable(std::move(inIterable)),
          containerResolver(std::move(inContainerResolver)),
          m_invoker(std::move(inInvoker))
    {}

    ReflectionTypeMethodInfo::ReflectionTypeMethodInfo()
        : property(),
          returnType(""),
          paramTypes({}),
          bIsIterable(false),
          bIsStatic(false),
          returnTypeIndex(std::nullopt),
          elementIndex(std::nullopt),
          returnSize(0),
          iterable({}),
          containerResolver({}),
          m_invoker({})
    {}

    std::any ReflectionTypeMethodInfo::invoke(void* inInstance, Params inParams) const
    {
        if (!m_invoker)
        {
            return {};
        }

        return m_invoker(inInstance, inParams);
    }

    const ReflectionTypeInfo* ReflectionTypeMethodInfo::findReturnType() const
    {
        if (returnTypeIndex.has_value())
        {
            if (const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(returnTypeIndex.value()))
            {
                return type;
            }
        }

        String name = returnType.trim();
        while (name.endsWith('*') || name.endsWith('&'))
        {
            name = name.substr(0, name.size() - 1).trim();
        }

        return name.isEmpty() ? nullptr : ReflectionTypeRegistry::sInstance().find(name);
    }

    const void* ReflectionTypeMethodInfo::unpack(const std::any& inValue) const
    {
        if (containerResolver)
        {
            return containerResolver(inValue);
        }

        const ReflectionTypeInfo* type = findReturnType();

        return type ? type->unpack(inValue) : nullptr;
    }

    String ReflectionTypeMethodInfo::toString(const std::any& inValue) const
    {
        return toString(inValue, String::sEmpty());
    }

    String ReflectionTypeMethodInfo::toString(const std::any& inValue, const String& inPath) const
    {
        if (!inValue.has_value())
        {
            return "";
        }

        const String path = inPath.startsWith('.') ? inPath.substr(1).trim() : inPath.trim();
        if (!path.isEmpty())
        {
            const void* instance = unpack(inValue);
            if (!instance)
            {
                return "";
            }

            const ReflectionTypeInfo* type = findReturnType();
            if (!type)
            {
                return "";
            }

            if (returnType.trim().endsWith('*'))
            {
                instance = *reinterpret_cast<void* const*>(instance);
                if (!instance)
                {
                    return "";
                }
            }

            const ReflectionFieldAccessor accessor = type->resolve(path);
            if (!accessor.isValid())
            {
                return "";
            }

            return accessor.toString(instance);
        }

        if (bIsIterable)
        {
            const ReflectionFieldAccessor accessor = makeAccessor(inValue);
            if (accessor.isValid())
            {
                return accessor.toString(nullptr);
            }
        }

        if (const ReflectionTypeInfo* type = findReturnType())
        {
            const String result = type->stringify(inValue);
            if (type->anyStringifier || type->stringifier)
            {
                return result;
            }
        }

        if (const String* value = std::any_cast<String>(&inValue))
        {
            return static_cast<String>(*value);
        }

        if (const std::string* value = std::any_cast<std::string>(&inValue))
        {
            return static_cast<String>(*value);
        }

        if (const FileSystem::Path* value = std::any_cast<FileSystem::Path>(&inValue))
        {
            return value->toString();
        }

        if (const char* value = std::any_cast<char>(&inValue))
        {
            return static_cast<String>(*value);
        }

        if (const bool* value = std::any_cast<bool>(&inValue))
        {
            return static_cast<String>(*value);
        }

        if (const int* value = std::any_cast<int>(&inValue))
        {
            return static_cast<String>(*value);
        }

        if (const long* value = std::any_cast<long>(&inValue))
        {
            return static_cast<String>(*value);
        }

        if (const float* value = std::any_cast<float>(&inValue))
        {
            return static_cast<String>(*value);
        }

        if (const double* value = std::any_cast<double>(&inValue))
        {
            return static_cast<String>(*value);
        }

        if (const std::uint64_t* value = std::any_cast<std::uint64_t>(&inValue))
        {
            return static_cast<String>(*value);
        }

        if (const std::uint32_t* value = std::any_cast<std::uint32_t>(&inValue))
        {
            return static_cast<String>(*value);
        }

        if (const std::uint16_t* value = std::any_cast<std::uint16_t>(&inValue))
        {
            return static_cast<String>(*value);
        }

        if (const std::uint8_t* value = std::any_cast<std::uint8_t>(&inValue))
        {
            return static_cast<String>(*value);
        }

        return "<" + returnType + ">";
    }

    bool ReflectionTypeMethodInfo::isIterable() const
    {
        return bIsIterable && static_cast<bool>(containerResolver) && iterable.sizeFunction && iterable.atFunction;
    }

    ReflectionFieldAccessor ReflectionTypeMethodInfo::makeAccessor(const std::any& inValue) const
    {
        if (!isIterable() || !inValue.has_value())
        {
            return {};
        }

        const void* container = containerResolver(inValue);
        if (!container)
        {
            return {};
        }

        return {
            0,
            0,
            returnSize > 0 ? returnSize : 1,
            {},
            returnType,
            returnTypeIndex,
            false,
            true,
            elementIndex,
            iterable,
            container,
            property.isTransient()
        };
    }

    bool ReflectionTypeMethodInfo::containsName(const String& inValue) const
    {
        return property.containsName(inValue);
    }

    const String& ReflectionTypeMethodInfo::getName() const
    {
        return property.getName();
    }

    bool ReflectionTypeMethodInfo::isTransient() const
    {
        return property.isTransient();
    }
}
