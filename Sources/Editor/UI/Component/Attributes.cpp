#include "Editor/UI/Component/Attributes.reflected.hpp"

#include "Editor/UI/Component/Dock/Header.hpp"
#include "Editor/UI/Component/Asset/Selector.hpp"
#include "Editor/UI/Component/Attributes/Tab.hpp"
#include "Editor/UI/Component/Vec/Vec3.hpp"
#include "Editor/UI/Prop.hpp"

namespace Editor
{
    Attributes::Attributes(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode),
          bIsItemSelected(false),
          attributeGroups({})
    {
        import <AttributesTab>();
        import <AssetSelector>();
        import <DockHeader>();
        import <Vec3>();

        load("Assets/Editor/UI/Components/Attributes.grid", "Assets/Editor/UI/Components/Attributes.decal");

        Prop::bind(this, ITEM_SELECTED_ATTRIBUTE, bIsItemSelected);
        Prop::bind(this, GROUPS_ATTRIBUTE, attributeGroups);
    }

    void Attributes::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        Prop::copy(this, ITEM_SELECTED_ATTRIBUTE, bIsItemSelected);
        Prop::copy(this, GROUPS_ATTRIBUTE, attributeGroups);
    }

    void Attributes::onAttributeCommit(Chicane::String inName, Chicane::String inValue)
    {
        Prop::invoke(this, ON_ATTRIBUTE_COMMIT_ATTRIBUTE, inName, inValue);
    }

    Chicane::String Attributes::getFieldDescription(Chicane::String inName)
    {
        for (const AttributeGroup& group : attributeGroups)
        {
            for (const AttributeField& field : group.fields)
            {
                if (field.name.equals(inName))
                {
                    return field.description;
                }
            }
        }

        return Chicane::String::empty();
    }
}
