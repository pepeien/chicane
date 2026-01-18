#include "Chicane/Box/Mesh.hpp"

#include <algorithm>

#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Texture.hpp"

#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        Mesh::Mesh(const FileSystem::Path& inFilepath)
            : Asset(inFilepath)
        {
            fetchGroups();
        }

        const std::vector<MeshGroup>& Mesh::getGroups() const
        {
            return m_groups;
        }

        void Mesh::setGroups(const std::vector<MeshGroup>& inGroups)
        {
            pugi::xml_node root = getXML();
            root.remove_children();

            for (const MeshGroup& group : inGroups)
            {
                appendGroup(group);
            }
        }

        void Mesh::appendGroup(const MeshGroup& inGroup)
        {
            if (!inGroup.isValid())
            {
                return;
            }

            String id = inGroup.getId();

            pugi::xml_node root = getXML();

            if (!Xml::isEmpty(root.find_child_by_attribute(GROUP_ID_ATTRIBUTE_NAME, id.toChar())))
            {
                throw std::runtime_error(
                    "A group with the ID " + inGroup.getId().toStandard() + " already exists"
                );
            }

            pugi::xml_node groupNode = root.append_child(GROUP_TAG);
            groupNode.append_attribute(GROUP_ID_ATTRIBUTE_NAME).set_value(id.toStandard());

            pugi::xml_node modelNode = groupNode.append_child(Model::TAG);
            Xml::addText(modelNode, inGroup.getModel());

            pugi::xml_node textureNode = groupNode.append_child(Texture::TAG);
            Xml::addText(textureNode, inGroup.getTexture());
        }

        void Mesh::updateGroup(const MeshGroup& inGroup)
        {
            if (inGroup.isValid())
            {
                return;
            }

            auto foundGroupEntry =
                std::find_if(m_groups.begin(), m_groups.end(), [inGroup](const MeshGroup& inValue) {
                    return inGroup.getId().equals(inValue.getId());
                });

            if (foundGroupEntry == m_groups.end())
            {
                throw std::runtime_error(
                    "The group " + inGroup.getId().toStandard() + " wasn't found"
                );
            }

            m_groups[foundGroupEntry - m_groups.begin()] = inGroup;

            String id = inGroup.getId();

            pugi::xml_node root = getXML();
            pugi::xml_node foundGroupNode =
                root.find_child_by_attribute(GROUP_ID_ATTRIBUTE_NAME, id.toChar());

            if (Xml::isEmpty(foundGroupNode))
            {
                appendGroup(inGroup);

                return;
            }

            pugi::xml_node modelNode = foundGroupNode.child(Model::TAG);
            Xml::addText(modelNode, inGroup.getModel());

            pugi::xml_node textureNode = foundGroupNode.child(Texture::TAG);
            Xml::addText(textureNode, inGroup.getTexture());
        }

        void Mesh::fetchGroups()
        {
            if (getFilepath().empty() || isEmpty())
            {
                return;
            }

            for (const auto& groupNode : getXML().children())
            {
                const String name = groupNode.name();

                if (!name.equals(GROUP_TAG))
                {
                    continue;
                }

                MeshGroup group = {};

                for (const auto& assetNode : groupNode.children())
                {
                    String currentTag = assetNode.name();

                    if (currentTag.equals(Model::TAG))
                    {
                        group.setModel(assetNode.child_value());

                        continue;
                    }

                    if (currentTag.equals(Texture::TAG))
                    {
                        group.setTexture(assetNode.child_value());

                        continue;
                    }
                }

                m_groups.push_back(group);
            }
        }
    }
}