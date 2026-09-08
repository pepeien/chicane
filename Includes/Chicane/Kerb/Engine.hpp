#pragma once

#include <memory>

#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Body.hpp"
#include "Chicane/Kerb/Body/CreateInfo.hpp"
#include "Chicane/Kerb/Gravity/Planet.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"
#include "Chicane/Kerb/Object/Layer.hpp"

namespace Chicane
{
    namespace Kerb
    {
        class CHICANE_KERB Engine
        {
            friend class Application;

        public:
            static Engine& getInstance();

            // Gravity
            static const Vec3& getZeroGravity();
            static const Vec3& getMercuryGravity();
            static const Vec3& getVenusGravity();
            static const Vec3& getEarthGravity();
            static const Vec3& getMoonGravity();
            static const Vec3& getMarsGravity();
            static const Vec3& getJupiterGravity();
            static const Vec3& getSaturnGravity();
            static const Vec3& getUranusGravity();
            static const Vec3& getNeptuneGravity();
            static const Vec3& getPlutoGravity();
            static const Vec3& getPlanetGravity(Planet inPlanet);

            static void setZeroGravity();
            static void setGravity(Planet inPlanet);
            static void setGravity(const Vec3& inValue);

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

            const Vec3& getGravity() const;

            Body createBody(const BodyCreateInfo& inCreateInfo);
            void activateBody(Body inBody);
            void deactivateBody(Body inBody);
            void destroyBody(Body inBody);

            void setBodyMotion(Body inBody, MotionType inType);
            void setBodyObjectLayer(Body inBody, ObjectLayer inLayer);
            void setBodyMass(Body inBody, float inMass, float inMassScale = 1.0f);
            void setBodyGravityFactor(Body inBody, float inFactor);
            float getBodyGravityFactor(Body inBody) const;

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
