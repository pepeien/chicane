#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Box.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    struct Transform
    {
        Vec<3, float> translation = Vec<3, float>(0.0f);
        Vec<3, float> rotation    = Vec<3, float>(0.0f);
        Vec<3, float> scale       = Vec<3, float>(1.0f);
    };

    class ActorComponent;
    class CameraComponent;

    class Actor
    {
    public:
        Actor();
        virtual ~Actor();

    public:
        virtual void onTick(float inDelta) { return; };

    public:
        bool canTick() const;
        void setCanTick(bool bInCanTick);

        const Mat<4, float>& getPosition() const;

        void setRelativeTranslation(const Vec<3, float>& inTranslation);
        void setAbsoluteTranslation(const Vec<3, float>& inTranslation);

        // Rotation values should be in degrees
        void setRelativeRotation(const Vec<3, float>& inRotation);
        void setAbsoluteRotation(const Vec<3, float>& inRotation);

        void setRelativeScale(const Vec<3, float>& inScale);
        void setAbsoluteScale(const Vec<3, float>& inScale);

        bool hasCamera() const;
        template<class T = CameraComponent>
        T* getCamera() const
        {
            if (!hasCamera())
            {
                return nullptr;
            }

            return dynamic_cast<T*>(m_camera);
        }

        const std::vector<ActorComponent*>& getComponents() const;

        bool hasMesh() const;
        const Box::Instance& getMesh() const;

    protected:
        void updateTranslation(const Vec<3, float>& inTranslation);
        void updateRotation(const Vec<3, float>& inRotation);
        void updateScale(const Vec<3, float>& inScale);

    protected:
        bool m_bCanTick;

        Mat<4, float> m_position;
        Transform m_transform;

        CameraComponent* m_camera;
        std::vector<ActorComponent*> m_components;

        Box::Instance m_mesh;
    };
} 