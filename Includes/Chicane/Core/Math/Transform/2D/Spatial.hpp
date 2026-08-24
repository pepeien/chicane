#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Transform/2D.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Reflection.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    struct CHICANE_CORE SpatialTransform2D : public Transform2D
    {
    public:
        SpatialTransform2D();

    protected:
        inline virtual void onTransformChanged() { return; }

    public:
        const Vec2& getRelativeTranslation() const;

        template <typename... Args>
        inline void addRelativeTranslation(Args... inArgs)
        {
            m_relative.addTranslation(std::forward<Args>(inArgs)...);

            refresh();
        }

        template <typename... Args>
        inline void setRelativeTranslation(Args... inArgs)
        {
            m_relative.setTranslation(std::forward<Args>(inArgs)...);

            refresh();
        }

        float getRelativeRotation() const;

        inline void addRelativeRotation(float inDegrees)
        {
            m_relative.addRotation(inDegrees);

            refresh();
        }

        inline void setRelativeRotation(float inDegrees)
        {
            m_relative.setRotation(inDegrees);

            refresh();
        }

        const Vec2& getRelativeScale() const;

        template <typename... Args>
        inline void addRelativeScale(Args... inArgs)
        {
            m_relative.addScale(std::forward<Args>(inArgs)...);

            refresh();
        }

        template <typename... Args>
        inline void setRelativeScale(Args... inArgs)
        {
            m_relative.setScale(std::forward<Args>(inArgs)...);

            refresh();
        }

        const Vec2& getAbsoluteTranslation() const;

        template <typename... Args>
        inline void addAbsoluteTranslation(Args... inArgs)
        {
            m_absolute.addTranslation(std::forward<Args>(inArgs)...);

            refresh();
        }

        template <typename... Args>
        inline void setAbsoluteTranslation(Args... inArgs)
        {
            m_absolute.setTranslation(std::forward<Args>(inArgs)...);

            refresh();
        }

        float getAbsoluteRotation() const;

        inline void addAbsoluteRotation(float inDegrees)
        {
            m_absolute.addRotation(inDegrees);

            refresh();
        }

        inline void setAbsoluteRotation(float inDegrees)
        {
            m_absolute.setRotation(inDegrees);

            refresh();
        }

        const Vec2& getAbsoluteScale() const;

        template <typename... Args>
        inline void addAbsoluteScale(Args... inArgs)
        {
            m_absolute.addScale(std::forward<Args>(inArgs)...);

            refresh();
        }

        template <typename... Args>
        inline void setAbsoluteScale(Args... inArgs)
        {
            m_absolute.setScale(std::forward<Args>(inArgs)...);

            refresh();
        }

        void lookAt(const Vec2& inTarget);

    protected:
        void refresh();

    private:
        Transform2D m_relative;
        Transform2D m_absolute;
    };
}
