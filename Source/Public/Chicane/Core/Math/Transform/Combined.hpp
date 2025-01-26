#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math/Transform/Instance.hpp"

namespace Chicane
{
    namespace Transform
    {
        struct Combined : protected Instance
        {
        public:
            // Transformation
            const Mat<4, float>& getTransformation() const;

            const Vec<3, float>& getTranslation() const;
            const Vec<3, float>& getRotation() const;
            const Vec<3, float>& getScale() const;

            const Instance& getRelativeTransform() const;
            void setRelativeTransform(const Instance& inTransform);

            const Vec<3, float>& getRelativeTranslation() const;
            void addRelativeTranslation(const Vec<3, float>& inTranslation);
            void setRelativeTranslation(const Vec<3, float>& inTranslation);

            const Vec<3, float>& getRelativeRotation() const;
            void addRelativeRotation(const Vec<3, float>& inRotation);
            void setRelativeRotation(const Vec<3, float>& inRotation);

            const Vec<3, float>& getRelativeScale() const;
            void addRelativeScale(const Vec<3, float>& inScale);
            void setRelativeScale(const Vec<3, float>& inScale);

            const Instance& getAbsoluteTransform() const;
            void setAbsoluteTransform(const Instance& inTransform);

            const Vec<3, float>& getAbsoluteTranslation() const;
            void addAbsoluteTranslation(const Vec<3, float>& inTranslation);
            void setAbsoluteTranslation(const Vec<3, float>& inTranslation);

            const Vec<3, float>& getAbsoluteRotation() const;
            void addAbsoluteRotation(const Vec<3, float>& inRotation);
            void setAbsoluteRotation(const Vec<3, float>& inRotation);

            const Vec<3, float>& getAbsoluteScale() const;
            void addAbsoluteScale(const Vec<3, float>& inScale);
            void setAbsoluteScale(const Vec<3, float>& inScale);

            // Orientation
            const Quat<float>& getOrientation() const;

            const Vec<3, float>& getRight() const;
            const Vec<3, float>& getForward() const;
            const Vec<3, float>& getUp() const;

        private:
            void refresh();

        private:
            Instance m_relative = {};
            Instance m_absolute = {};
        };
    }
}