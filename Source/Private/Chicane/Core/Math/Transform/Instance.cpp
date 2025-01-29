#include "Chicane/Core/Math/Transform/Instance.hpp"

static const Chicane::Mat<4, float> BASE_MAT(1.0f);

namespace Chicane
{
    namespace Transform
    {
        const Mat<4, float>& Instance::getMatrix() const
        {
            return m_matrix;
        }

        void Instance::setTransform(const Instance& inTransform)
        {
            setTranslation(inTransform.getTranslation());
            setRotation(inTransform.getRotation());
            setScale(inTransform.getScale());
        }

        const Vec<3, float>& Instance::getTranslation() const
        {
            return m_translation;
        }

        void Instance::addTranslation(const Vec<3, float>& inTranslation)
        {
            setTranslation(m_translation + inTranslation);
        }

        void Instance::setTranslation(const Vec<3, float>& inTranslation)
        {
            m_translation = inTranslation;

            refresh();
        }

        const Vec<3, float>& Instance::getRotation() const
        {
            return m_rotation;
        }

        void Instance::addRotation(const Vec<3, float>& inRotation)
        {
            setRotation(m_rotation + inRotation);
        }

        void Instance::setRotation(const Vec<3, float>& inRotation)
        {
            m_rotation = inRotation;

            if (std::abs(m_rotation.x) > 360.0f)
            {
                m_rotation.x = 0.0f;
            }

            if (std::abs(m_rotation.y) > 360.0f)
            {
                m_rotation.y = 0.0f;
            }

            if (std::abs(m_rotation.z) > 360.0f)
            {
                m_rotation.z = 0.0f;
            }

            refresh();
        }

        const Vec<3, float>& Instance::getScale() const
        {
            return m_scale;
        }

        void Instance::addScale(const Vec<3, float>& inScale)
        {
            setScale(m_scale + inScale);
        }

        void Instance::setScale(const Vec<3, float>& inScale)
        {
            m_scale = inScale;

            refresh();
        }

        const Vec<3, float>& Instance::getRight() const
        {
            return m_right;
        }

        const Vec<3, float>& Instance::getForward() const
        {
            return m_forward;
        }

        const Vec<3, float>& Instance::getUp() const
        {
            return m_up;
        }

        void Instance::refreshOrientation(const Quat<float>& inOrientation)
        {
            m_forward = glm::rotate(inOrientation, FORWARD_DIRECTION);
            m_right   = glm::rotate(inOrientation, RIGHT_DIRECTION);
            m_up      = glm::rotate(inOrientation, UP_DIRECTION);
        }

        void Instance::refreshTransform(const Quat<float>& inOrientation)
        {
            m_matrix  = glm::translate(BASE_MAT, m_translation); // Transalate
            m_matrix *= glm::toMat4(inOrientation);              // Rotate
            m_matrix  = glm::scale(m_matrix, m_scale);           // Scale
        }

        void Instance::refresh()
        {
            Quat<float> orientation = glm::normalize(
                glm::angleAxis(glm::radians(m_rotation.z), UP_DIRECTION) *    // Yaw
                glm::angleAxis(glm::radians(m_rotation.x), RIGHT_DIRECTION) * // Pitch
                glm::angleAxis(glm::radians(m_rotation.y), FORWARD_DIRECTION) // Roll
            );

            refreshOrientation(orientation);
            refreshTransform(orientation);
        }
    }
}