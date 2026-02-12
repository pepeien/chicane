#include "Chicane/Box/Mesh/Group.hpp"

namespace Chicane
{
    namespace Box
    {
        bool MeshGroup::isValid() const
        {
            return !m_id.isEmpty() && m_model.isValid() && m_texture.isValid();
        }

        const String& MeshGroup::getId() const
        {
            return m_id;
        }

        void MeshGroup::setId(const String& inId)
        {
            m_id = inId;
        }

        const AssetReference& MeshGroup::getModel() const
        {
            return m_model;
        }

        void MeshGroup::setModel(const pugi::xml_node& inNode)
        {
            m_model.setFrom(inNode);
        }

        void MeshGroup::setModel(const String& inSource, const String& inReference)
        {
            m_model.setSource(inSource.toStandard());
            m_model.setReference(inReference);
        }

        const AssetReference& MeshGroup::getTexture() const
        {
            return m_texture;
        }

        void MeshGroup::setTexture(const pugi::xml_node& inNode)
        {
            m_texture.setFrom(inNode);
        }

        void MeshGroup::setTexture(const String& inSource, const String& inReference)
        {
            m_texture.setSource(inSource.toStandard());
            m_texture.setReference(inReference);
        }
    }
}