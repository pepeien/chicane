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

    const ReflectionMethodInfo* ReflectionTypeInfo::findMethod(const String& inName) const
    {
        for (const ReflectionMethodInfo& method : methods)
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
            throw std::runtime_error("TypeInfo::resolve — empty path on type [" + name + "]");
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
                throw std::runtime_error(
                    "TypeInfo::resolve — field [" + part + "] not found on type [" + currentType->name + "] (path: [" +
                    inAccessor + "])"
                );
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
                    throw std::runtime_error(
                        "TypeInfo::resolve — field [" + part + "] on type [" + currentType->name +
                        "] is not a reflected type, cannot dot-chain further"
                        " (path: [" +
                        inAccessor + "])"
                    );
                }

                if (currentField->bIsPointer && !crossedPointer)
                {
                    crossedPointer = true;
                }

                currentType = ReflectionTypeRegistry::getInstance().find(currentField->typeIndex.value());

                if (!currentType)
                {
                    throw std::runtime_error(
                        "TypeInfo::resolve — type [" + currentField->typeName +
                        "] is marked reflected but is not registered"
                        " (path: [" +
                        inAccessor + "])"
                    );
                }
            }
        }

        return {offset, ptrOffset, field->size, field->names, field->typeName, field->typeIndex, crossedPointer};
    }
}