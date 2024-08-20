#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Box.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    struct Transform
    {
        Vec<float>::Three translation = Vec<float>::Three(0.0f);
        Vec<float>::Three rotation    = Vec<float>::Three(0.0f);
        Vec<float>::Three scale       = Vec<float>::Three(1.0f);
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
        bool canTick();
        void setCanTick(bool bInCanTick);

        const Mat<float>::Four& getPosition();

        void setRelativeTranslation(const Vec<float>::Three& inTranslation);
        void setAbsoluteTranslation(const Vec<float>::Three& inTranslation);

        // Rotation values should be in degrees
        void setRelativeRotation(const Vec<float>::Three& inRotation);
        void setAbsoluteRotation(const Vec<float>::Three& inRotation);

        void setRelativeScale(const Vec<float>::Three& inScale);
        void setAbsoluteScale(const Vec<float>::Three& inScale);

        bool hasCamera();

        template<class T = CameraComponent>
        T* getCamera()
        {
            if (!hasCamera())
            {
                return nullptr;
            }

            return dynamic_cast<T*>(m_camera);
        }

        const std::vector<ActorComponent*>& getComponents();

        const Box::Instance& getMesh();

    protected:
        void setTranslation(const Mat<float>::Four& inBase, const Vec<float>::Three& inTranslation);
        void setRotation(const Mat<float>::Four& inBase, const Vec<float>::Three& inRotation);
        void setScale(const Mat<float>::Four& inBase, const Vec<float>::Three& inScale);

    protected:
        bool m_bCanTick;

        Mat<float>::Four m_position;
        Transform m_transform;

        CameraComponent* m_camera;
        std::vector<ActorComponent*> m_components;

        Box::Instance m_mesh;
    };
} 