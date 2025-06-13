#include "Chicane/Core/Math/Transform/Instance.hpp"

static const Chicane::Mat4 BASE_MAT(1.0f);

namespace Chicane
{
    namespace Transform
    {
        const Mat4& Instance::getMatrix() const
        {
            return m_matrix;
        }

        void Instance::setMatrix(const Mat4& inMatrix)
        {
            m_matrix = inMatrix;
        }

        void Instance::setTransform(const Instance& inTransform)
        {
            setTranslation(inTransform.getTranslation());
            setRotation(inTransform.getRotation());
            setScale(inTransform.getScale());
        }

        const Vec3& Instance::getTranslation() const
        {
            return m_translation;
        }

        void Instance::addTranslation(const Vec3& inTranslation)
        {
            setTranslation(m_translation + inTranslation);
        }

        void Instance::setTranslation(const Vec3& inTranslation)
        {
            m_translation = inTranslation;

            refresh();
        }

        const Vec3& Instance::getRotation() const
        {
            return m_rotation;
        }

        void Instance::addRotation(const Vec3& inRotation)
        {
            setRotation(m_rotation + inRotation);
        }

        void Instance::setRotation(const Vec3& inRotation)
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

        const Vec3& Instance::getScale() const
        {
            return m_scale;
        }

        void Instance::addScale(const Vec3& inScale)
        {
            setScale(m_scale + inScale);
        }

        void Instance::setScale(const Vec3& inScale)
        {
            m_scale = inScale;

            refresh();
        }

        const Vec3& Instance::getRight() const
        {
            return m_right;
        }

        const Vec3& Instance::getForward() const
        {
            return m_forward;
        }

        const Vec3& Instance::getUp() const
        {
            return m_up;
        }

        void Instance::refreshOrientation(const QuatFloat& inOrientation)
        {
            m_forward = glm::rotate(inOrientation, Vec3::Forward);
            m_right   = glm::rotate(inOrientation, Vec3::Right);
            m_up      = glm::rotate(inOrientation, Vec3::Up);
        }

        void Instance::refreshTransform(const QuatFloat& inOrientation)
        {
            m_matrix  = glm::translate(BASE_MAT, m_translation); // Transalate
            m_matrix *= glm::toMat4(inOrientation);              // Rotate
            m_matrix  = glm::scale(m_matrix, m_scale);           // Scale
        }

        void Instance::refresh()
        {
            QuatFloat orientation = glm::normalize(
                glm::angleAxis(glm::radians(m_rotation.z), Vec3::Up) *    // Yaw
                glm::angleAxis(glm::radians(m_rotation.x), Vec3::Right) * // Pitch
                glm::angleAxis(glm::radians(m_rotation.y), Vec3::Forward) // Roll
            );

            refreshOrientation(orientation);
            refreshTransform(orientation);
        }
    }
}