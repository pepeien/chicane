#include "Chicane/Core/Reflection/Type/Method/Info.hpp"

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

namespace Chicane
{
    ReflectionTypeMethodInfo::ReflectionTypeMethodInfo(
        String                  inName,
        String                  inReturnType,
        std::vector<String>     inParamTypes,
        Invoker                 inInvoker,
        bool                    bInIsIterable,
        TypeIndex               inReturnTypeIndex,
        TypeIndex               inElementIndex,
        std::size_t             inReturnSize,
        ReflectionFieldIterable inIterable,
        ContainerResolver       inContainerResolver
    )
        : name(std::move(inName)),
          returnType(std::move(inReturnType)),
          paramTypes(std::move(inParamTypes)),
          bIsIterable(bInIsIterable),
          returnTypeIndex(std::move(inReturnTypeIndex)),
          elementIndex(std::move(inElementIndex)),
          returnSize(inReturnSize),
          iterable(std::move(inIterable)),
          containerResolver(std::move(inContainerResolver)),
          m_invoker(std::move(inInvoker))
    {}

    ReflectionTypeMethodInfo::ReflectionTypeMethodInfo()
        : name(""),
          returnType(""),
          paramTypes({}),
          bIsIterable(false),
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

    String ReflectionTypeMethodInfo::toString(const std::any& inValue) const
    {
        if (!inValue.has_value())
        {
            return "";
        }

        if (bIsIterable)
        {
            const ReflectionFieldAccessor accessor = makeAccessor(inValue);
            if (accessor.isValid())
            {
                return accessor.toString(nullptr);
            }
        }

        if (const String* value = std::any_cast<String>(&inValue))
        {
            return *value;
        }

        if (const std::string* value = std::any_cast<std::string>(&inValue))
        {
            return *value;
        }

        if (const Vec2* value = std::any_cast<Vec2>(&inValue))
        {
            return value->toString();
        }

        if (const Vec3* value = std::any_cast<Vec3>(&inValue))
        {
            return value->toString();
        }

        if (const Vec4* value = std::any_cast<Vec4>(&inValue))
        {
            return value->toString();
        }

        if (const bool* value = std::any_cast<bool>(&inValue))
        {
            return *value ? "true" : "false";
        }

        if (const char* value = std::any_cast<char>(&inValue))
        {
            return String(1, *value);
        }

        if (const int* value = std::any_cast<int>(&inValue))
        {
            return std::to_string(*value);
        }

        if (const long* value = std::any_cast<long>(&inValue))
        {
            return std::to_string(*value);
        }

        if (const float* value = std::any_cast<float>(&inValue))
        {
            return std::to_string(*value);
        }

        if (const double* value = std::any_cast<double>(&inValue))
        {
            return std::to_string(*value);
        }

        if (const std::uint64_t* value = std::any_cast<std::uint64_t>(&inValue))
        {
            return std::to_string(*value);
        }

        if (const std::uint32_t* value = std::any_cast<std::uint32_t>(&inValue))
        {
            return std::to_string(*value);
        }

        if (const std::uint16_t* value = std::any_cast<std::uint16_t>(&inValue))
        {
            return std::to_string(*value);
        }

        if (const std::uint8_t* value = std::any_cast<std::uint8_t>(&inValue))
        {
            return std::to_string(*value);
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
            container
        };
    }
}
