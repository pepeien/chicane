#include "Editor/UI/Component/Attributes.reflected.hpp"

#include <Chicane/Core/Reflection/Type/Field/Acessor.hpp>
#include <Chicane/Grid/Component.hpp>

#include "Editor/UI/Component/Dock/Header.hpp"
#include "Editor/UI/Component/Asset/Selector.hpp"
#include "Editor/UI/Component/Attributes/Tab.hpp"
#include "Editor/UI/Component/Vec/Vec3.hpp"
#include "Editor/UI/Prop.hpp"

namespace Editor
{
    Attributes::Attributes(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode)
    {
        import <AttributesTab>();
        import <AssetSelector>();
        import <DockHeader>();
        import <Vec3>();

        load("Assets/Editor/UI/Components/Attributes/Index.grid", "Assets/Editor/UI/Components/Attributes/Index.decal");
    }

    void Attributes::onAttributeCommit(Chicane::String inName, Chicane::String inValue)
    {
        Prop::invoke(this, ON_ATTRIBUTE_COMMIT_ATTRIBUTE, inName, inValue);
    }

    const AttributeGroup::List* Attributes::groups() const
    {
        for (const Chicane::Grid::Component* node = this; node != nullptr;
             node                                 = node->hasParent() ? node->getParent() : nullptr)
        {
            const Chicane::ReflectionFieldAccessor accessor = node->getField("attributeGroups");
            if (accessor.isValid() && accessor.isType<AttributeGroup::List>())
            {
                const void* instance =
                    accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(node);

                return accessor.getValue<AttributeGroup::List>(instance);
            }

            if (node->isRoot())
            {
                break;
            }
        }

        return nullptr;
    }

    Chicane::String Attributes::getFieldDescription(Chicane::String inName)
    {
        const AttributeGroup::List* groups = this->groups();
        if (!groups)
        {
            return Chicane::String::sEmpty();
        }

        for (const AttributeGroup& group : *groups)
        {
            for (const AttributeField& field : group.fields)
            {
                if (field.name.equals(inName))
                {
                    return field.description;
                }
            }
        }

        return Chicane::String::sEmpty();
    }
}
