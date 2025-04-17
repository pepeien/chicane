#include "Box/Asset/Mesh/Group.hpp"

#include "Core.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Mesh
        {
            bool Group::isValid() const
            {
                return !m_id.empty() && FileSystem::exists(m_model) && FileSystem::exists(m_texture);
            }

            const std::string& Group::getId() const
            {
                return m_id;
            }

            void Group::setId(const std::string& inId)
            {
                m_id = inId;
            }

            const std::string& Group::getModel() const
            {
                return m_model;
            }

            void Group::setModel(const std::string& inFilepath)
            {
                if (inFilepath.empty())
                {
                    return;
                }

                m_model = String::trim(inFilepath);
            }

            const std::string& Group::getTexture() const
            {
                return m_texture;
            }

            void Group::setTexture(const std::string& inFilepath)
            {
                if (inFilepath.empty())
                {
                    return;
                }

                m_texture = String::trim(inFilepath);
            }
        }
    }
}