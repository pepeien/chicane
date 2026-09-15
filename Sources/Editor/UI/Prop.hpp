#pragma once

#include <type_traits>
#include <utility>

#include <Chicane/Core/Reflection/Type/Field/Acessor.hpp>
#include <Chicane/Core/Reflection/Type/Method.hpp>
#include <Chicane/Core/Reflection/Type/Registry.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Grid.hpp>
#include <Chicane/Grid/Component.hpp>

namespace Editor
{
    namespace Prop
    {
        inline Chicane::String unwrap(const Chicane::String& inValue)
        {
            const Chicane::String value = inValue.trim();
            if (value.startsWith(Chicane::Grid::REFERENCE_VALUE_OPENING) &&
                value.endsWith(Chicane::Grid::REFERENCE_VALUE_CLOSING))
            {
                return value.getBetween(Chicane::Grid::REFERENCE_VALUE_OPENING, Chicane::Grid::REFERENCE_VALUE_CLOSING)
                    .trim();
            }

            return value;
        }

        inline Chicane::String methodName(const Chicane::String& inValue)
        {
            Chicane::String   name = unwrap(inValue);
            const std::size_t open = name.firstOf(Chicane::Grid::METHOD_PARAMS_OPENING);
            if (open != Chicane::String::npos)
            {
                name = name.substr(0, open);
            }

            return name.trim();
        }

        template <typename T>
        inline void copy(Chicane::Grid::Component* inComponent, const Chicane::String& inAttribute, T& outValue)
        {
            if (!inComponent)
            {
                return;
            }

            const Chicane::String raw = inComponent->getAttribute(inAttribute);
            if (raw.isEmpty())
            {
                return;
            }

            if (!raw.contains(Chicane::Grid::REFERENCE_VALUE_OPENING))
            {
                if constexpr (std::is_same_v<T, Chicane::String>)
                {
                    outValue = raw;
                }

                return;
            }

            const Chicane::String id = unwrap(raw);
            if (id.isEmpty())
            {
                return;
            }

            for (Chicane::Grid::Component* node = inComponent->getParent(); node != nullptr; node = node->getParent())
            {
                const Chicane::ReflectionFieldAccessor accessor = node->getField(id);
                if (accessor.isValid() && accessor.isType<T>())
                {
                    const void* instance =
                        accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(node);

                    if (const T* value = accessor.getValue<T>(instance))
                    {
                        outValue = *value;
                    }

                    return;
                }

                if (node->isRoot())
                {
                    break;
                }
            }
        }

        template <typename T>
        inline void pushParam(Chicane::ReflectionTypeMethod& ioMethod, T&& inValue)
        {
            using Decayed = std::decay_t<T>;

            if constexpr (std::is_pointer_v<Decayed>)
            {
                ioMethod.addParam(const_cast<void*>(static_cast<const void*>(inValue)));
            }
            else
            {
                ioMethod.addParam(std::forward<T>(inValue));
            }
        }

        template <typename... Args>
        inline void invoke(Chicane::Grid::Component* inComponent, const Chicane::String& inAttribute, Args&&... inArgs)
        {
            if (!inComponent)
            {
                return;
            }

            const Chicane::String name = methodName(inComponent->getAttribute(inAttribute));
            if (name.isEmpty())
            {
                return;
            }

            for (Chicane::Grid::Component* node = inComponent->getParent(); node != nullptr; node = node->getParent())
            {
                const Chicane::ReflectionTypeInfo* type =
                    Chicane::ReflectionTypeRegistry::getInstance().find(typeid(*node));
                if (type)
                {
                    if (const Chicane::ReflectionTypeMethodInfo* method = type->findMethod(name))
                    {
                        Chicane::ReflectionTypeMethod result(method);
                        result.bind(node);
                        (pushParam(result, std::forward<Args>(inArgs)), ...);
                        result.invoke();

                        return;
                    }
                }

                if (node->isRoot())
                {
                    break;
                }
            }
        }
    }
}
