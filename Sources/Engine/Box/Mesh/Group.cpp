#include "Chicane/Box/Mesh/Group.hpp"

namespace Chicane
{
    namespace Box
    {
        bool MeshGroup::isValid() const
        {
            return !m_id.isEmpty() && FileSystem::exists(m_model.toStandard()) &&
                   FileSystem::exists(m_texture.toStandard());
        }

        const String& MeshGroup::getId() const
        {
            return m_id;
        }

        void MeshGroup::setId(const String& inId)
        {
            m_id = inId;
        }

        const String& MeshGroup::getModel() const
        {
            return m_model;
        }

        void MeshGroup::setModel(const String& inFilepath)
        {
            if (inFilepath.isEmpty())
            {
                return;
            }

            m_model = inFilepath.trim();
        }

        const String& MeshGroup::getTexture() const
        {
            return m_texture;
        }

        void MeshGroup::setTexture(const String& inFilepath)
        {
            if (inFilepath.isEmpty())
            {
                return;
            }

            m_texture = inFilepath.trim();
        }
    }
}