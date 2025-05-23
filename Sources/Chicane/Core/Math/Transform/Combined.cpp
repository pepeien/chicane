#include "Chicane/Core/Math/Transform/Combined.hpp"

namespace Chicane
{
    namespace Transform
    {
        const Mat4& Combined::getMatrix() const
        {
            return m_matrix;
        }

        const Vec3& Combined::getTranslation() const
        {
            return m_translation;
        }

        const Vec3& Combined::getRotation() const
        {
            return m_rotation;
        }

        const Vec3& Combined::getScale() const
        {
            return m_scale;
        }

        const Instance& Combined::getRelativeTransform() const
        {
            return m_relative;
        }

        void Combined::setRelativeTransform(const Instance& inTransform)
        {
            m_relative.setTransform(inTransform);

            refresh();
        }

        const Vec3& Combined::getRelativeTranslation() const
        {
            return m_relative.getTranslation();
        }

        void Combined::addRelativeTranslation(const Vec3& inTranslation)
        {
            m_relative.addRotation(inTranslation);

            refresh();
        }

        void Combined::setRelativeTranslation(const Vec3& inTranslation)
        {
            m_relative.setTranslation(inTranslation);

            refresh();
        }

        const Vec3& Combined::getRelativeRotation() const
        {
            return m_relative.getRotation();
        }

        void Combined::addRelativeRotation(const Vec3& inRotation)
        {
            m_relative.addRotation(inRotation);

            refresh();
        }

        void Combined::setRelativeRotation(const Vec3& inRotation)
        {
            m_relative.setRotation(inRotation);

            refresh();
        }

        const Vec3& Combined::getRelativeScale() const
        {
            return m_relative.getScale();
        }

        void Combined::addRelativeScale(const Vec3& inScale)
        {
            m_relative.addScale(inScale);

            refresh();
        }

        void Combined::setRelativeScale(const Vec3& inScale)
        {
            m_relative.setScale(inScale);

            refresh();
        }

        const Instance& Combined::getAbsoluteTransform() const
        {
            return m_absolute;
        }

        void Combined::setAbsoluteTransform(const Instance& inTransform)
        {
            m_absolute.setTransform(inTransform);

            refresh();
        }

        const Vec3& Combined::getAbsoluteTranslation() const
        {
            return m_absolute.getTranslation();
        }

        void Combined::addAbsoluteTranslation(const Vec3& inTranslation)
        {
            m_absolute.addTranslation(inTranslation);

            refresh();
        }

        void Combined::setAbsoluteTranslation(const Vec3& inTranslation)
        {
            m_absolute.setTranslation(inTranslation);

            refresh();
        }

        const Vec3& Combined::getAbsoluteRotation() const
        {
            return m_absolute.getRotation();
        }

        void Combined::addAbsoluteRotation(const Vec3& inRotation)
        {
            m_absolute.addRotation(inRotation);

            refresh();
        }

        void Combined::setAbsoluteRotation(const Vec3& inRotation)
        {
            m_absolute.setRotation(inRotation);

            refresh();
        }

        const Vec3& Combined::getAbsoluteScale() const
        {
            return m_absolute.getScale();
        }

        void Combined::addAbsoluteScale(const Vec3& inScale)
        {
            m_absolute.addScale(inScale);

            refresh();
        }

        void Combined::setAbsoluteScale(const Vec3& inScale)
        {
            m_absolute.setScale(inScale);

            refresh();
        }

        const Vec3& Combined::getRight() const
        {
            return m_right;
        }

        const Vec3& Combined::getForward() const
        {
            return m_forward;
        }

        const Vec3& Combined::getUp() const
        {
            return m_up;
        }

        void Combined::refresh()
        {
            // Transformation
            m_matrix = m_absolute.getMatrix() * m_relative.getMatrix();

            m_translation = m_absolute.getTranslation() + m_relative.getTranslation();
            m_rotation    = m_absolute.getRotation() + m_relative.getRotation();
            m_scale       = m_absolute.getScale() * m_relative.getScale();

            refreshOrientation(glm::quat_cast(m_matrix));
        }
    }
}