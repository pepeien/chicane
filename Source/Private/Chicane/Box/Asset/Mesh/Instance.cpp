#include "Chicane/Box/Asset/Mesh/Instance.hpp"

#include "Chicane/Box/Asset/Model.hpp"
#include "Chicane/Box/Asset/Texture.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Mesh
        {
            static constexpr const char* GROUP_TAG               = "Group";
            static constexpr const char* GROUP_ID_ATTRIBUTE_NAME = "id";

            Instance::Instance(const std::string& inFilepath)
                : Asset::Instance(inFilepath)
            {
                fetchGroups();
            }

            const std::vector<Group>& Instance::getGroups() const
            {
                return m_groups;
            }

            void Instance::setGroups(const std::vector<Group>& inGroups)
            {
                pugi::xml_node root = getXMLRoot();
                root.remove_children();

                for (const Group& group : inGroups)
                {
                    appendGroup(group);
                }
            }

            void Instance::appendGroup(const Group& inGroup)
            {
                if (!inGroup.isValid())
                {
                    return;
                }

                std::string id = inGroup.getId();

                pugi::xml_node root = getXMLRoot();

                if (
                    !XML::isEmpty(
                        root.find_child_by_attribute(
                            GROUP_ID_ATTRIBUTE_NAME,
                            id.c_str()
                        )
                    )
                )
                {
                    throw std::runtime_error("A group with the ID " + inGroup.getId() + " already exists");
                }

                pugi::xml_node groupNode = root.append_child(GROUP_TAG);
                groupNode.append_attribute(GROUP_ID_ATTRIBUTE_NAME).set_value(id);

                pugi::xml_node modelNode = groupNode.append_child(Model::TAG);
                XML::addText(modelNode, inGroup.getModel());

                pugi::xml_node textureNode = groupNode.append_child(Texture::TAG);
                XML::addText(textureNode, inGroup.getTexture());
            }

            void Instance::updateGroup(const Group& inGroup)
            {
                if (inGroup.isValid())
                {
                    return;
                }

                auto foundGroupEntry = std::find_if(
                    m_groups.begin(),
                    m_groups.end(),
                    [inGroup](const auto& A) { return Utils::areEquals(A.getId(), inGroup.getId()); }
                );

                if (foundGroupEntry == m_groups.end())
                {
                    throw std::runtime_error("The group " + inGroup.getId() + " wasn't found");
                }

                m_groups[foundGroupEntry - m_groups.begin()] = inGroup;

                std::string id = inGroup.getId();

                pugi::xml_node root           = getXMLRoot();
                pugi::xml_node foundGroupNode = root.find_child_by_attribute(
                    GROUP_ID_ATTRIBUTE_NAME,
                    id.c_str()
                );

                if (XML::isEmpty(foundGroupNode))
                {
                    appendGroup(inGroup);

                    return;
                }

                pugi::xml_node modelNode = foundGroupNode.child(Model::TAG);
                XML::addText(modelNode, inGroup.getModel());

                pugi::xml_node textureNode = foundGroupNode.child(Texture::TAG);
                XML::addText(textureNode, inGroup.getTexture());
            }

            void Instance::fetchGroups()
            {
                if (getFilepath().empty() || isXMLEmpty())
                {
                    return;
                }

                for (const auto& groupNode : getXMLRoot().children())
                {
                    if (!Utils::areEquals(groupNode.name(), GROUP_TAG))
                    {
                        continue;
                    }

                    Group group {};

                    for (const auto& assetNode : groupNode.children())
                    {
                        std::string currentTag = assetNode.name();

                        if (Utils::areEquals(currentTag, Model::TAG))
                        {
                            group.setModel(
                                Utils::trim(
                                    assetNode.child_value()
                                )
                            );

                            continue;
                        }

                        if (Utils::areEquals(currentTag, Texture::TAG))
                        {
                            group.setTexture(
                                Utils::trim(
                                    assetNode.child_value()
                                )
                            );

                            continue;
                        }
                    }

                    m_groups.push_back(group);
                }
            }
        }
    }
}