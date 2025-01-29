#include "Chicane/Core/Math/Transform/Combined.hpp"

namespace Chicane
{
    namespace Transform
    {
        const Mat<4, float>& Combined::getMatrix() const
        {
            return m_matrix;
        }

        const Vec<3, float>& Combined::getTranslation() const
        {
            return m_translation;
        }

        const Vec<3, float>& Combined::getRotation() const
        {
            return m_rotation;
        }

        const Vec<3, float>& Combined::getScale() const
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

        const Vec<3, float>& Combined::getRelativeTranslation() const
        {
            return m_relative.getTranslation();
        }

        void Combined::addRelativeTranslation(const Vec<3, float>& inTranslation)
        {
            m_relative.addRotation(inTranslation);

            refresh();
        }

        void Combined::setRelativeTranslation(const Vec<3, float>& inTranslation)
        {
            m_relative.setTranslation(inTranslation);

            refresh();
        }

        const Vec<3, float>& Combined::getRelativeRotation() const
        {
            return m_relative.getRotation();
        }

        void Combined::addRelativeRotation(const Vec<3, float>& inRotation)
        {
            m_relative.addRotation(inRotation);

            refresh();
        }

        void Combined::setRelativeRotation(const Vec<3, float>& inRotation)
        {
            m_relative.setRotation(inRotation);

            refresh();
        }

        const Vec<3, float>& Combined::getRelativeScale() const
        {
            return m_relative.getScale();
        }

        void Combined::addRelativeScale(const Vec<3, float>& inScale)
        {
            m_relative.addScale(inScale);

            refresh();
        }

        void Combined::setRelativeScale(const Vec<3, float>& inScale)
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

        const Vec<3, float>& Combined::getAbsoluteTranslation() const
        {
            return m_absolute.getTranslation();
        }

        void Combined::addAbsoluteTranslation(const Vec<3, float>& inTranslation)
        {
            m_absolute.addTranslation(inTranslation);

            refresh();
        }

        void Combined::setAbsoluteTranslation(const Vec<3, float>& inTranslation)
        {
            m_absolute.setTranslation(inTranslation);

            refresh();
        }

        const Vec<3, float>& Combined::getAbsoluteRotation() const
        {
            return m_absolute.getRotation();
        }

        void Combined::addAbsoluteRotation(const Vec<3, float>& inRotation)
        {
            m_absolute.addRotation(inRotation);

            refresh();
        }

        void Combined::setAbsoluteRotation(const Vec<3, float>& inRotation)
        {
            m_absolute.setRotation(inRotation);

            refresh();
        }

        const Vec<3, float>& Combined::getAbsoluteScale() const
        {
            return m_absolute.getScale();
        }

        void Combined::addAbsoluteScale(const Vec<3, float>& inScale)
        {
            m_absolute.addScale(inScale);

            refresh();
        }

        void Combined::setAbsoluteScale(const Vec<3, float>& inScale)
        {
            m_absolute.setScale(inScale);

            refresh();
        }

        const Vec<3, float>& Combined::getRight() const
        {
            return m_right;
        }

        const Vec<3, float>& Combined::getForward() const
        {
            return m_forward;
        }

        const Vec<3, float>& Combined::getUp() const
        {
            return m_up;
        }

        void Combined::refresh()
        {
            // Transformation
            m_matrix = m_absolute.getMatrix() * m_relative.getMatrix();

            m_translation = m_absolute.getTranslation() + m_relative.getTranslation();
            m_rotation    = m_absolute.getRotation() * m_relative.getRotation();
            m_scale       = m_absolute.getScale() * m_relative.getScale();

            refreshOrientation(glm::quat_cast(m_matrix));
        }
    }
}