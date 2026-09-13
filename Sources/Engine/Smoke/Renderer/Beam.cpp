#include "Chicane/Smoke/Renderer/Beam.reflected.hpp"

#include <algorithm>
#include <cmath>

#include "Chicane/Smoke/Parse.hpp"
#include "Chicane/Smoke/Renderer/Beam.hpp"

namespace Chicane
{
    namespace Smoke
    {
        RendererBeam::RendererBeam(const pugi::xml_node& inNode)
            : RendererBeam()
        {
            m_tag = inNode.name();
            width = parseFloat(inNode, WIDTH_ATTRIBUTE_NAME, width);
            blend = parseString(inNode, BLEND_ATTRIBUTE_NAME, blend);
            color = parseColor(inNode, COLOR_ATTRIBUTE_NAME, color);
        }

        RendererBeam::RendererBeam()
            : Module(),
              width(WIDTH_DEFAULT_VALUE),
              blend(BLEND_TYPE_ADDITIVE),
              color(Vec4(COLOR_R_DEFAULT_VALUE, COLOR_G_DEFAULT_VALUE, COLOR_B_DEFAULT_VALUE, COLOR_A_DEFAULT_VALUE))
        {}

        void RendererBeam::collect(
            const Particle::List& inParticles, const PlayInfo& inPlay, Particle::List& outDraws
        ) const
        {
            const float additive = blend.equals(BLEND_TYPE_ADDITIVE, BLEND_TYPE_ADD, BLEND_TYPE_ADDITIVE_LOWER)
                                       ? BLEND_ADDITIVE_VALUE
                                       : BLEND_ALPHA_VALUE;

            if (inPlay.bHasBeam)
            {
                Vec3        delta  = inPlay.destination - inPlay.origin;
                const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
                if (length > LENGTH_MIN_VALUE)
                {
                    Particle beam;
                    beam.position = Vec3(
                        (inPlay.origin.x + inPlay.destination.x) * 0.5f,
                        (inPlay.origin.y + inPlay.destination.y) * 0.5f,
                        (inPlay.origin.z + inPlay.destination.z) * 0.5f
                    );
                    beam.axis     = Vec3(delta.x / length, delta.y / length, delta.z / length);
                    beam.size     = Vec2(length, width);
                    beam.sizeEnd  = beam.size;
                    beam.color    = color;
                    beam.additive = additive;
                    beam.lifetime = LIFETIME_DEFAULT_VALUE;
                    outDraws.push_back(beam);
                }
            }

            for (const Particle& particle : inParticles)
            {
                if (!particle.bAlive)
                {
                    continue;
                }

                Particle draw = particle;
                draw.additive = additive;
                if (draw.axis.x == 0.0f && draw.axis.y == 0.0f && draw.axis.z == 0.0f && inPlay.bHasBeam)
                {
                    Vec3        delta  = inPlay.destination - inPlay.origin;
                    const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
                    if (length > LENGTH_MIN_VALUE)
                    {
                        draw.axis = Vec3(delta.x / length, delta.y / length, delta.z / length);
                    }
                }
                outDraws.push_back(draw);
            }
        }
    }
}
