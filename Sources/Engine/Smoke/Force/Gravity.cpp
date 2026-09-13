#include "Chicane/Smoke/Force/Gravity.reflected.hpp"

#include "Chicane/Smoke/Force/Gravity.hpp"
#include "Chicane/Smoke/Parse.hpp"

namespace Chicane
{
    namespace Smoke
    {
        ForceGravity::ForceGravity()
            : Module(),
              value(Vec3(0.0f, 0.0f, Z_DEFAULT_VALUE))
        {}

        ForceGravity::ForceGravity(const pugi::xml_node& inNode)
            : ForceGravity()
        {
            m_tag = inNode.name();
            value = parseVec3(inNode, VALUE_ATTRIBUTE_NAME, value);

            const pugi::xml_attribute z = Xml::getAttribute(Z_ATTRIBUTE_NAME, inNode);
            if (!z.empty())
            {
                value.z = z.as_float(value.z);
            }
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
