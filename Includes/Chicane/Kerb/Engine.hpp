#pragma once

#include <memory>

#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Body.hpp"
#include "Chicane/Kerb/Body/CreateInfo.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"

namespace Chicane
{
    namespace Kerb
    {
        class CHICANE_KERB Engine
        {
            friend class Application;

        public:
            static Engine& getInstance();

        public:
            Engine(const Engine&)            = delete;
            Engine& operator=(const Engine&) = delete;

            Engine(Engine&&)            = delete;
            Engine& operator=(Engine&&) = delete;

        private:
            Engine();
            ~Engine();

        public:
            void tick(float inDeltaTime); // seconds

            Body createBody(const BodyCreateInfo& inCreateInfo);
            void activateBody(Body inBody);
            void deactivateBody(Body inBody);
            void destroyBody(Body inBody);

            void setBodyMotion(Body inBody, MotionType inType);

            void addBodyImpulse(Body inBody, const Vec3& inDirection, float inForce, const Vec3& inLocation);

            Vec3 getBodyLinearVelocity(Body inBody) const;
            void setBodyLinearVelocity(Body inBody, const Vec3& inVelocity);
            void setBodyHorizontalVelocity(Body inBody, const Vec3& inVelocity);

            Transform getBodyTransform(Body inBody, bool bInInterpolate = false) const;
            void setBodyTransform(Body inBody, const Transform& inValue);

            BodyPolygon getBodyPolygon(Body inBody) const;

        private:
            struct Implementation;

            std::unique_ptr<Implementation> m_implementation;
        };
    }
}
