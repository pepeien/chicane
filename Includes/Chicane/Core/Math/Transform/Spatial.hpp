#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    struct CHICANE_CORE SpatialTransform : public Transform
    {
    public:
        SpatialTransform();

    protected:
        inline virtual void onRefresh() { return; }

    public:
        const Vec3& getRelativeTranslation() const;

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

        const Rotator& getRelativeRotation() const;

        template <typename... Args>
        inline void addRelativeRotation(Args... inArgs)
        {
            m_relative.addLocalRotation(std::forward<Args>(inArgs)...);

            refresh();
        }

        template <typename... Args>
        inline void setRelativeRotation(Args... inArgs)
        {
            m_relative.setRotation(std::forward<Args>(inArgs)...);

            refresh();
        }

        const Vec3& getRelativeScale() const;

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

        const Vec3& getAbsoluteTranslation() const;

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

        const Rotator& getAbsoluteRotation() const;

        template <typename... Args>
        inline void addAbsoluteRotation(Args... inArgs)
        {
            m_absolute.addWorldRotation(std::forward<Args>(inArgs)...);

            refresh();
        }

        template <typename... Args>
        inline void setAbsoluteRotation(Args... inArgs)
        {
            m_absolute.setRotation(std::forward<Args>(inArgs)...);

            refresh();
        }

        const Vec3& getAbsoluteScale() const;

        template <typename... Args>
        void addAbsoluteScale(Args... inArgs)
        {
            m_absolute.addScale(std::forward<Args>(inArgs)...);

            refresh();
        }

        template <typename... Args>
        void setAbsoluteScale(Args... inArgs)
        {
            m_absolute.setScale(std::forward<Args>(inArgs)...);

            refresh();
        }

        void lookAt(const Vec3& inTarget);

    private:
        void refresh();

    private:
        Transform m_relative;
        Transform m_absolute;
    };
}