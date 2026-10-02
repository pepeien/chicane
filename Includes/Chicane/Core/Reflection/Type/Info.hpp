#pragma once

#include <any>
#include <cstddef>
#include <functional>
#include <optional>
#include <type_traits>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Property.hpp"
#include "Chicane/Core/Reflection/Type/Field/Acessor.hpp"
#include "Chicane/Core/Reflection/Type/Field/Info.hpp"
#include "Chicane/Core/Reflection/Type/Method/Info.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionTypeInfo
    {
    public:
        using TypeIdex       = std::optional<std::type_index>;
        using Names          = ReflectionProperty::Names;
        using Fields         = std::vector<ReflectionFieldInfo>;
        using Methods        = std::vector<ReflectionTypeMethodInfo>;
        using Constructor    = std::function<void*(std::vector<std::any>)>;
        using Constructors   = std::vector<Constructor>;
        using Stringifier    = std::function<String(const void*)>;
        using AnyStringifier = std::function<String(const std::any&)>;

    public:
        static constexpr inline const char OBJECT_SEPARATOR = '.';

    public:
        ReflectionTypeInfo(
            ReflectionProperty  inProperty,
            std::size_t         inSize,
            TypeIdex            inTypeIndex,
            const Constructors& inConstructors,
            const Methods&      inMethods,
            const Fields&       inFields,
            Stringifier         inStringifier,
            AnyStringifier      inAnyStringifier
        );
        ReflectionTypeInfo();

        template <typename T>
        static Stringifier sMakeStringifier()
        {
            return sMakeStringifier<T>(std::bool_constant<std::is_convertible_v<const T&, String>>{});
        }

        template <typename T>
        static AnyStringifier sMakeAnyStringifier()
        {
            return sMakeAnyStringifier<T>(std::bool_constant<std::is_convertible_v<const T&, String>>{});
        }

    public:
        bool containsName(const String& inValue) const;
        const String& getName() const;
        const Names& getNames() const;
        const String& getGroup() const;
        const String& getDescription() const;
        bool isTransient() const;

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
                catch (const std::runtime_error& error)
                {
                    const std::string message = error.what();
                    if (message.find("Missing reflected constructor") != std::string::npos)
                    {
                        continue;
                    }

                    throw std::runtime_error("Failed to construct [" + getName() + "]: " + message);
                }
            }

            throw std::runtime_error("No matching reflected constructor for type [" + getName() + "]");
        }

    public:
        ReflectionProperty property;
        std::size_t        size;
        TypeIdex           typeIndex;
        Constructors       constructors;
        Methods            methods;
        Fields             fields;
        Stringifier        stringifier;
        AnyStringifier     anyStringifier;

    private:
        template <typename T>
        static String sStringify(const void* inInstance)
        {
            return static_cast<String>(*static_cast<const T*>(inInstance));
        }

        template <typename T>
        static String sStringifyAny(const std::any& inValue)
        {
            const T* value = std::any_cast<T>(&inValue);
            if (!value)
            {
                return {};
            }

            return static_cast<String>(*value);
        }

        template <typename T>
        static Stringifier sMakeStringifier(std::true_type)
        {
            return &sStringify<T>;
        }

        template <typename T>
        static Stringifier sMakeStringifier(std::false_type)
        {
            return {};
        }

        template <typename T>
        static AnyStringifier sMakeAnyStringifier(std::true_type)
        {
            return &sStringifyAny<T>;
        }

        template <typename T>
        static AnyStringifier sMakeAnyStringifier(std::false_type)
        {
            return {};
        }
    };
}
