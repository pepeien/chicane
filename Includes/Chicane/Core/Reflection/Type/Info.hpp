#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Type/Field/Acessor.hpp"
#include "Chicane/Core/Reflection/Type/Field/Info.hpp"
#include "Chicane/Core/Reflection/Type/Method/Info.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionTypeInfo
    {
    public:
        using TypeIdex     = std::optional<std::type_index>;
        using Names        = std::vector<String>;
        using Fields       = std::vector<ReflectionFieldInfo>;
        using Methods      = std::vector<ReflectionTypeMethodInfo>;
        using Constructor  = std::function<void*(std::vector<std::any>)>;
        using Constructors = std::vector<Constructor>;

    public:
        ReflectionTypeInfo(
            Names               inNames,
            std::size_t         inSize,
            TypeIdex            inTypeIndex,
            const Constructors& inConstructors,
            const Methods&      inMethods,
            const Fields&       inFields
        );
        ReflectionTypeInfo();

    public:
        bool containsName(const String& inValue) const;
        const String& getName() const;

        const ReflectionFieldInfo* findField(const String& inName) const;

        const ReflectionTypeMethodInfo* findMethod(const String& inName) const;

        ReflectionFieldAccessor resolve(const String& inAccessor) const;

        template <typename T>
        T* create(std::vector<std::any> inArgs = {}) const
        {
            for (const Constructor constructor : constructors)
            {
                try
                {
                    if (void* instance = constructor(inArgs))
                    {
                        return static_cast<T*>(instance);
                    }
                }
                catch (const std::bad_any_cast&)
                {
                    continue;
                }
                catch (const std::runtime_error&)
                {
                    continue;
                }
            }

            throw std::runtime_error("No matching reflected constructor for type [" + getName() + "]");
        }

    public:
        Names        names;
        std::size_t  size;
        TypeIdex     typeIndex;
        Constructors constructors;
        Methods      methods;
        Fields       fields;
    };
}
