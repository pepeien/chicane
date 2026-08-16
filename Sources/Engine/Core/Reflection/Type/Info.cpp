#include "Chicane/Core/Reflection/Type/Info.hpp"

#include "Chicane/Core/Reflection/Type/Registry.hpp"

namespace Chicane
{

    ReflectionTypeInfo::ReflectionTypeInfo(
        const String&       inName,
        std::size_t         inSize,
        TypeIdex            inTypeIndex,
        const Constructors& inConstructors,
        const Methods&      inMethods,
        const Fields&       inFields
    )
        : name(std::move(inName)),
          size(inSize),
          typeIndex(inTypeIndex),
          constructors(std::move(inConstructors)),
          methods(std::move(inMethods)),
          fields(std::move(inFields))
    {}

    ReflectionTypeInfo::ReflectionTypeInfo()
        : name(""),
          size(0),
          typeIndex(std::nullopt),
          constructors({}),
          methods({}),
          fields({})
    {}

    const ReflectionFieldInfo* ReflectionTypeInfo::findField(const String& inName) const
    {
        for (const ReflectionFieldInfo& field : fields)
        {
            if (field.containsName(inName))
            {
                return &field;
            }
        }

        return nullptr;
    }

    const ReflectionTypeMethodInfo* ReflectionTypeInfo::findMethod(const String& inName) const
    {
        for (const ReflectionTypeMethodInfo& method : methods)
        {
            if (method.name.equals(inName))
            {
                return &method;
            }
        }

        return nullptr;
    }

    ReflectionFieldAccessor ReflectionTypeInfo::resolve(const String& inAccessor) const
    {
        std::vector<String> parts = inAccessor.split('.');

        if (parts.empty())
        {
            return {};
        }

        const ReflectionTypeInfo*  currentType    = this;
        std::size_t                offset         = 0;
        std::size_t                ptrOffset      = 0;
        bool                       crossedPointer = false;
        const ReflectionFieldInfo* field          = nullptr;

        for (std::size_t i = 0; i < parts.size(); ++i)
        {
            const String&              part         = parts[i];
            const ReflectionFieldInfo* currentField = currentType->findField(part);

            if (!currentField)
            {
                return {};
            }

            if (crossedPointer)
            {
                ptrOffset += currentField->offset;
            }
            else
            {
                offset += currentField->offset;
            }

            field = currentField;

            if (i < parts.size() - 1)
            {
                if (!currentField->bIsReflected || !currentField->typeIndex.has_value())
                {
                    return {};
                }

                if (currentField->bIsPointer && !crossedPointer)
                {
                    crossedPointer = true;
                }

                currentType = ReflectionTypeRegistry::getInstance().find(currentField->typeIndex.value());

                if (!currentType)
                {
                    return {};
                }
            }
        }

        return {
            offset,
            ptrOffset,
            field->size,
            field->names,
            field->typeName,
            field->typeIndex,
            crossedPointer,
            field->bIsIterable,
            field->elementIndex,
            field->iterable,
            nullptr
        };
    }
}