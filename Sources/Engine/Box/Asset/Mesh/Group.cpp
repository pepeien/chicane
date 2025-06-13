#include "Chicane/Box/Asset/Mesh/Group.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Mesh
        {
            bool Group::isValid() const
            {
                return !m_id.isEmpty() &&
                       FileSystem::exists(m_model.toStandard()) &&
                       FileSystem::exists(m_texture.toStandard());
            }

            const String& Group::getId() const
            {
                return m_id;
            }

            void Group::setId(const String& inId)
            {
                m_id = inId;
            }

            const String& Group::getModel() const
            {
                return m_model;
            }

            void Group::setModel(const String& inFilepath)
            {
                if (inFilepath.isEmpty())
                {
                    return;
                }

                m_model = inFilepath.trim();
            }

            const String& Group::getTexture() const
            {
                return m_texture;
            }

            void Group::setTexture(const String& inFilepath)
            {
                if (inFilepath.isEmpty())
                {
                    return;
                }

                m_texture = inFilepath.trim();
            }
        }
    }
}