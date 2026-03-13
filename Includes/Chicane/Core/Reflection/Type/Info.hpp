#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Field/Acessor.hpp"
#include "Chicane/Core/Reflection/Field/Info.hpp"
#include "Chicane/Core/Reflection/Method/Info.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionTypeInfo
    {
    public:
        using TypeIdex     = std::optional<std::type_index>;
        using Fields       = std::vector<ReflectionFieldInfo>;
        using Methods      = std::vector<ReflectionMethodInfo>;
        using Constructor  = std::function<void*(std::vector<std::any>)>;
        using Constructors = std::vector<Constructor>;

    public:
        ReflectionTypeInfo(
            const String&       inName,
            std::size_t         inSize,
            TypeIdex            inTypeIndex,
            const Constructors& inConstructors,
            const Methods&      inMethods,
            const Fields&       inFields
        );
        ReflectionTypeInfo();

    public:
        const ReflectionFieldInfo* findField(const String& inName) const;

        const ReflectionMethodInfo* findMethod(const String& inName) const;

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

            throw std::runtime_error("No matching reflected constructor for type [" + name + "]");
        }

    public:
        String       name;
        std::size_t  size;
        TypeIdex     typeIndex;
        Constructors constructors;
        Methods      methods;
        Fields       fields;
    };
}