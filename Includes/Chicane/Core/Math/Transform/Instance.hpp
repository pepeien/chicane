#pragma once

#include "Chicane.hpp"
#include "Chicane/Core/Math/Mat.hpp"
#include "Chicane/Core/Math/Quat.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    namespace Transform
    {
        struct Instance
        {
        public:
            // Transformation
            const Mat<4, float>& getMatrix() const;
            void setMatrix(const Mat<4, float>&  inMatrix);

            void setTransform(const Instance& inTransform);

            const Vec<3, float>& getTranslation() const;
            void addTranslation(const Vec<3, float>& inTranslation);
            void setTranslation(const Vec<3, float>& inTranslation);

            const Vec<3, float>& getRotation() const;
            void addRotation(const Vec<3, float>& inRotation);
            void setRotation(const Vec<3, float>& inRotation);

            const Vec<3, float>& getScale() const;
            void addScale(const Vec<3, float>& inScale);
            void setScale(const Vec<3, float>& inScale);

            // Orientation
            const Vec<3, float>& getRight() const;
            const Vec<3, float>& getForward() const;
            const Vec<3, float>& getUp() const;

        protected:
            void refreshOrientation(const Quat<float>& inOrientation);
            void refreshTransform(const Quat<float>& inOrientation);
            void refresh();

        protected:
            // Transformation
            Mat<4, float> m_matrix         = Mat<4, float>(1.0f);
            Vec<3, float> m_translation    = Vec<3, float>(0.0f);
            Vec<3, float> m_rotation       = Vec<3, float>(0.0f);
            Vec<3, float> m_scale          = Vec<3, float>(1.0f);

            // Orientation
            Vec<3, float> m_forward        = FORWARD_DIRECTION;
            Vec<3, float> m_up             = UP_DIRECTION;
            Vec<3, float> m_right          = RIGHT_DIRECTION;
        };
    }
}