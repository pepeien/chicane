#include "Chicane/Core/Reflection/Type/Info.hpp"

#include "Chicane/Core/Reflection/Type/Registry.hpp"

namespace Chicane
{
    const ReflectionFieldInfo* ReflectionTypeInfo::findField(const String& fieldName) const
    {
        for (const auto& f : fields)
        {
            if (f.name == fieldName)
            {
                return &f;
            }
        }

        return nullptr;
    }

    const ReflectionMethodInfo* ReflectionTypeInfo::findMethod(const String& methodName) const
    {
        for (const auto& m : methods)
        {
            if (m.name == methodName)
            {
                return &m;
            }
        }

        return nullptr;
    }

    ReflectionFieldAccessor ReflectionTypeInfo::resolve(const String& path) const
    {
        std::vector<String> parts = path.split('.');

        if (parts.empty())
        {
            throw std::runtime_error("TypeInfo::resolve — empty path on type '" + name + "'");
        }

        const ReflectionTypeInfo*  currentType = this;
        std::size_t                totalOffset = 0;
        const ReflectionFieldInfo* lastField   = nullptr;

        for (std::size_t i = 0; i < parts.size(); ++i)
        {
            const String&              part = parts[i];
            const ReflectionFieldInfo* fi   = currentType->findField(part);

            if (!fi)
            {
                std::ostringstream err;
                err << "TypeInfo::resolve — field '" << part << "' not found on type '" << currentType->name
                    << "' (path: '" << path << "')";
                throw std::runtime_error(err.str());
            }

            totalOffset += fi->offset;
            lastField = fi;

            if (i < parts.size() - 1)
            {
                if (!fi->isReflected || !fi->typeIndex.has_value())
                {
                    std::ostringstream err;
                    err << "TypeInfo::resolve — field '" << part << "' on type '" << currentType->name
                        << "' is not a reflected type, cannot dot-chain further"
                        << " (path: '" << path << "')";
                    throw std::runtime_error(err.str());
                }

                currentType = ReflectionTypeRegistry::get().find(fi->typeIndex.value());

                if (!currentType)
                {
                    throw std::runtime_error(
                        "TypeInfo::resolve — type [" + fi->typeName + "] is marked reflected but is not registered" +
                        " (path: [" + path + "])"
                    );
                }
            }
        }

        return ReflectionFieldAccessor{
            totalOffset,
            lastField->size,
            lastField->name,
            lastField->typeName,
            lastField->typeIndex
        };
    }
}