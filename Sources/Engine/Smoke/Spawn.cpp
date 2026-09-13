#include "Chicane/Smoke/Spawn.reflected.hpp"

#include <algorithm>
#include <cmath>

#include "Chicane/Core/Xml.hpp"

#include "Chicane/Smoke/Parse.hpp"
#include "Chicane/Smoke/Spawn.hpp"

namespace Chicane
{
    namespace Smoke
    {
        Spawn::Spawn()
            : Module(),
              shape(SHAPE_TYPE_POINT),
              anchor(ANCHOR_TYPE_ORIGIN),
              rate(0.0f),
              burst(0),
              lifetime(LIFETIME_DEFAULT_VALUE),
              speed(0.0f),
              size(SIZE_DEFAULT_VALUE),
              sizeEnd(SIZE_DEFAULT_VALUE),
              spread(0.0f),
              radius(0.0f),
              color(Vec4(1.0f)),
              colorEnd(Vec4(1.0f)),
              additive(BLEND_ADDITIVE_VALUE)
        {}

        Spawn::Spawn(const pugi::xml_node& inNode)
            : Spawn()
        {
            m_tag  = inNode.name();
            shape  = parseString(inNode, SHAPE_ATTRIBUTE_NAME, shape);
            anchor = parseString(inNode, ANCHOR_ATTRIBUTE_NAME, anchor);
            if (Xml::getAttribute(ORIGIN_ATTRIBUTE_NAME, inNode).empty() == false)
            {
                anchor = parseString(inNode, ORIGIN_ATTRIBUTE_NAME, anchor);
            }
            rate     = parseFloat(inNode, RATE_ATTRIBUTE_NAME, rate);
            burst    = parseUint(inNode, BURST_ATTRIBUTE_NAME, burst);
            lifetime = parseRange(inNode, LIFETIME_ATTRIBUTE_NAME, lifetime);
            speed    = parseRange(inNode, SPEED_ATTRIBUTE_NAME, speed);
            size     = parseRange(inNode, SIZE_ATTRIBUTE_NAME, size);
            sizeEnd  = parseRange(inNode, SIZE_END_ATTRIBUTE_NAME, sizeEnd);
            spread   = parseRange(inNode, SPREAD_ATTRIBUTE_NAME, spread);
            radius   = parseRange(inNode, RADIUS_ATTRIBUTE_NAME, radius);
            color    = parseColor(inNode, COLOR_ATTRIBUTE_NAME, color);
            colorEnd = parseColor(inNode, COLOR_END_ATTRIBUTE_NAME, colorEnd);

            const String blend = parseString(inNode, BLEND_ATTRIBUTE_NAME, "");
            if (!blend.isEmpty())
            {
                additive = blend.equals(BLEND_TYPE_ADDITIVE, BLEND_TYPE_ADD, BLEND_TYPE_ADDITIVE_LOWER)
                               ? BLEND_ADDITIVE_VALUE
                               : BLEND_ALPHA_VALUE;
            }
        }

        void Spawn::emit(Particle::List& outParticles, const PlayInfo& inPlay, std::uint32_t inCount)
        {
            Vec3 origin = inPlay.origin;
            if (!inPlay.bHasBeam)
            {
                origin = inPlay.transform.getTranslation();
            }
            if (anchor.equals(
                    ANCHOR_TYPE_DESTINATION,
                    ANCHOR_TYPE_IMPACT,
                    ANCHOR_TYPE_DESTINATION_LOWER,
                    ANCHOR_TYPE_END
                ))
            {
                origin = inPlay.destination;
            }

            Vec3  beam       = Vec3::Zero();
            float beamLength = 0.0f;
            if (inPlay.bHasBeam)
            {
                beam       = inPlay.destination - inPlay.origin;
                beamLength = std::sqrt(beam.x * beam.x + beam.y * beam.y + beam.z * beam.z);
                if (beamLength > LENGTH_MIN_VALUE)
                {
                    beam = Vec3(beam.x / beamLength, beam.y / beamLength, beam.z / beamLength);
                }
            }

            for (std::uint32_t i = 0; i < inCount; ++i)
            {
                Particle particle;
                particle.lifetime   = std::max(LIFETIME_MIN_VALUE, randomFloat(lifetime.from, lifetime.to));
                particle.colorStart = color;
                particle.colorEnd   = colorEnd;
                particle.color      = color;
                particle.size       = Vec2(randomFloat(size.from, size.to));
                particle.sizeStart  = particle.size;
                particle.sizeEnd    = Vec2(randomFloat(sizeEnd.from, sizeEnd.to));
                particle.additive   = additive;
                particle.rotation   = randomFloat(0.0f, Math::TWO_PI);

                Vec3 offset = Vec3::Zero();
                if (shape.equals(SHAPE_TYPE_SPHERE))
                {
                    offset = randomDirection() * randomFloat(0.0f, randomFloat(radius.from, radius.to));
                }
                else if (shape.equals(SHAPE_TYPE_BOX) && inPlay.bHasBeam && beamLength > 0.0f)
                {
                    const float t = randomFloat(0.0f, 1.0f);
                    offset        = beam * (beamLength * t);
                    particle.axis = beam;
                }
                else if (shape.equals(SHAPE_TYPE_BOX))
                {
                    const float extent = randomFloat(radius.from, radius.to);
                    offset =
                        Vec3(randomFloat(-extent, extent), randomFloat(-extent, extent), randomFloat(-extent, extent));
                }
                else if (shape.equals(SHAPE_TYPE_CONE) && inPlay.bHasBeam)
                {
                    offset            = Vec3::Zero();
                    const float angle = randomFloat(spread.from, spread.to) * Math::DEG_TO_RAD;
                    Vec3        dir   = beam;
                    if (angle > 0.0f)
                    {
                        dir = (beam + randomDirection() * std::sin(angle)).normalize();
                    }
                    particle.velocity = dir * randomFloat(speed.from, speed.to);
                }

                particle.position = origin + offset;

                if (particle.velocity.x == 0.0f && particle.velocity.y == 0.0f && particle.velocity.z == 0.0f)
                {
                    const float magnitude = randomFloat(speed.from, speed.to);
                    if (magnitude > 0.0f)
                    {
                        particle.velocity = randomDirection() * magnitude;
                    }
                }

                outParticles.push_back(particle);
            }
        }
    }
}
