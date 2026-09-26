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
    class Instance;

    namespace Kerb
    {
        class CHICANE_KERB Engine
        {
            friend class ::Chicane::Instance;

        public:
            static Engine& sInstance();

            // Gravity
            static const Vec3& sGetZeroGravity();
            static const Vec3& sGetMercuryGravity();
            static const Vec3& sGetVenusGravity();
            static const Vec3& sGetEarthGravity();
            static const Vec3& sGetMoonGravity();
            static const Vec3& sGetMarsGravity();
            static const Vec3& sGetJupiterGravity();
            static const Vec3& sGetSaturnGravity();
            static const Vec3& sGetUranusGravity();
            static const Vec3& sGetNeptuneGravity();
            static const Vec3& sGetPlutoGravity();
            static const Vec3& sGetPlanetGravity(Planet inPlanet);

            static void sSetZeroGravity();
            static void sSetGravity(Planet inPlanet);
            static void sSetGravity(const Vec3& inValue);

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
