#include "Chicane/Box/Assets/Mesh.hpp"

#include "Chicane/Box/Assets/Model.hpp"
#include "Chicane/Box/Assets/Texture.hpp"
#include "Chicane/Core/Utils.hpp"
#include "Chicane/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        constexpr auto GROUP_TAG = "Group";

        Mesh::Mesh(const std::string& inFilepath)
            : Asset(inFilepath)
        {
            fetchGroups();
        }

        const std::string& Mesh::Group::getModel() const
        {
            return m_model;
        }

        void Mesh::Group::setModel(const std::string& inFilepath)
        {
            m_model = Utils::trim(inFilepath);
        }

        const std::string& Mesh::Group::getTexture() const
        {
            return m_texture;
        }

        void Mesh::Group::setTexture(const std::string& inFilepath)
        {
            m_texture = Utils::trim(inFilepath);
        }

        const std::vector<Mesh::Group>& Mesh::getGroups() const
        {
            return m_groups;
        }

        void Mesh::fetchGroups()
        {
            if (m_header.filepath.empty() || m_xml.empty())
            {
                return;
            }

            for (const auto& groupNode : m_xml.first_child().children())
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