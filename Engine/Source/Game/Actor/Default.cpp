#include "Default.hpp"

namespace Engine
{
    namespace Actor
    {
        Default::Default()
            : m_position(glm::mat4(1.0f)),
            m_transform({}),
            m_mesh({})
        {}

        glm::mat4 Default::getPosition()
        {
            return m_position;
        }

        void Default::translate(const glm::vec3& inTranslation)
        {
            m_transform.translation = inTranslation;

            m_position = glm::translate(m_position, inTranslation);
        }

        void Default::rotate(const glm::vec3& inRotation)
        {
            
        }

        void Default::scale(const glm::vec3& inScale)
        {
            m_transform.scale = inScale;

            m_position = glm::scale(m_position, inScale);
        }

        void Default::setMesh(const Mesh& inMesh)
        {
            m_mesh = inMesh;
        }
    }
}