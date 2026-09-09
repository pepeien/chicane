#include "Chicane/Box/Mesh.hpp"

#include <algorithm>

#include "Chicane/Box/Asset/Preview.hpp"
#include "Chicane/Box/Model.hpp"

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
            for (pugi::xml_node child = root.first_child(); child;)
            {
                pugi::xml_node next = child.next_sibling();
                if (!String(child.name()).equals(AssetPreview::TAG))
                {
                    root.remove_child(child);
                }

                child = next;
            }

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
                throw std::runtime_error("A group with the ID " + inGroup.getId().toStandard() + " already exists");
            }

            pugi::xml_node groupNode = root.append_child(GROUP_TAG);
            groupNode.append_attribute(GROUP_ID_ATTRIBUTE_NAME).set_value(id.toStandard());

            // Model
            pugi::xml_node modelNode = groupNode.append_child(Model::TAG);
            inGroup.getModel().saveTo(modelNode);

            // Textures
            inGroup.saveTextures(groupNode);

            // Transform
            inGroup.saveTransform(groupNode);
        }

        void Mesh::updateGroup(const MeshGroup& inGroup)
        {
            if (!inGroup.isValid())
            {
                return;
            }

            auto foundGroupEntry = std::find_if(
                m_groups.begin(),
                m_groups.end(),
                [inGroup](const MeshGroup& inValue) { return inGroup.getId().equals(inValue.getId()); }
            );

            if (foundGroupEntry == m_groups.end())
            {
                throw std::runtime_error("The group " + inGroup.getId().toStandard() + " wasn't found");
            }

            m_groups[foundGroupEntry - m_groups.begin()] = inGroup;

            String id = inGroup.getId();

            pugi::xml_node root           = getXML();
            pugi::xml_node foundGroupNode = root.find_child_by_attribute(GROUP_ID_ATTRIBUTE_NAME, id.toChar());

            if (Xml::isEmpty(foundGroupNode))
            {
                appendGroup(inGroup);

                return;
            }

            // Model
            pugi::xml_node modelNode = foundGroupNode.child(Model::TAG);
            inGroup.getModel().saveTo(modelNode);

            // Textures
            inGroup.saveTextures(foundGroupNode);

            // Transform
            inGroup.saveTransform(foundGroupNode);
        }

        void Mesh::fetchGroups()
        {
            if (getFilepath().isEmpty() || isXMLEmpty())
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

                MeshGroup group;
                group.setId(Xml::getAttribute(GROUP_ID_ATTRIBUTE_NAME, groupNode).as_string());
                group.setTransform(groupNode);

                for (const auto& assetNode : groupNode.children())
                {
                    const String currentTag = assetNode.name();

                    if (currentTag.equals(Model::TAG))
                    {
                        group.setModel(assetNode);

                        continue;
                    }

                    const TextureMap textureMap = toTextureMap(currentTag);
                    if (textureMap != TextureMap::Count)
                    {
                        group.setTexture(textureMap, assetNode);

                        continue;
                    }
                }

                m_groups.push_back(group);
            }
        }
    }
}