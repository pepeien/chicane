#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Box.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    struct Transform
    {
        Math<float, 3>::Vec translation = Math<float, 3>::Vec(0.0f);
        Math<float, 3>::Vec rotation    = Math<float, 3>::Vec(0.0f);
        Math<float, 3>::Vec scale       = Math<float, 3>::Vec(1.0f);
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

        const Math<float, 4>::Mat& getPosition() const;

        void setRelativeTranslation(const Math<float, 3>::Vec& inTranslation);
        void setAbsoluteTranslation(const Math<float, 3>::Vec& inTranslation);

        // Rotation values should be in degrees
        void setRelativeRotation(const Math<float, 3>::Vec& inRotation);
        void setAbsoluteRotation(const Math<float, 3>::Vec& inRotation);

        void setRelativeScale(const Math<float, 3>::Vec& inScale);
        void setAbsoluteScale(const Math<float, 3>::Vec& inScale);

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
        void updateTranslation(const Math<float, 3>::Vec& inTranslation);
        void updateRotation(const Math<float, 3>::Vec& inRotation);
        void updateScale(const Math<float, 3>::Vec& inScale);

    protected:
        bool m_bCanTick;

        Math<float, 4>::Mat m_position;
        Transform m_transform;

        CameraComponent* m_camera;
        std::vector<ActorComponent*> m_components;

        Box::Instance m_mesh;
    };
} 