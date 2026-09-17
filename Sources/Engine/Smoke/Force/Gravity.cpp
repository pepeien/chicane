#include "Chicane/Smoke/Force/Gravity.reflected.hpp"

#include "Chicane/Smoke/Force/Gravity.hpp"

namespace Chicane
{
    namespace Smoke
    {
        ForceGravity::ForceGravity()
            : Module(),
              value(Vec3(0.0f, 0.0f, Z_DEFAULT_VALUE))
        {
            watchRefresh(VALUE_ATTRIBUTE_NAME);
            watchRefresh(Z_ATTRIBUTE_NAME);
        }

        ForceGravity::ForceGravity(const XmlNode& inNode)
            : ForceGravity()
        {
            parse(inNode);
        }

        void ForceGravity::onAttributeRefresh()
        {
            m_tag   = getSource().getName();
            value   = getVec3(VALUE_ATTRIBUTE_NAME, value);
            value.z = getFloat(Z_ATTRIBUTE_NAME, value.z);
        }

        void ForceGravity::tick(float inDeltaTime, Particle::List& outParticles, const PlayInfo&)
        {
            for (Particle& particle : outParticles)
            {
                if (!particle.bAlive)
                {
                    continue;
                }

                particle.velocity = particle.velocity + value * inDeltaTime;
            }
        }
    }
}
