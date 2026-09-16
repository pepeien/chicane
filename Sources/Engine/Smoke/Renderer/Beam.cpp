#include "Chicane/Smoke/Renderer/Beam.reflected.hpp"

#include <algorithm>
#include <cmath>

#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Smoke
    {
        RendererBeam::RendererBeam(const XmlNode& inNode)
            : RendererBeam()
        {
            parse(inNode);
        }

        void RendererBeam::refresh()
        {
            m_tag = getSource().getName();
            width = getFloat(WIDTH_ATTRIBUTE_NAME, width);
            blend = getString(BLEND_ATTRIBUTE_NAME, blend);
            color = getColor(COLOR_ATTRIBUTE_NAME, color);
        }

        RendererBeam::RendererBeam()
            : Module(),
              width(WIDTH_DEFAULT_VALUE),
              blend(BLEND_TYPE_ADDITIVE),
              color(Vec4(COLOR_R_DEFAULT_VALUE, COLOR_G_DEFAULT_VALUE, COLOR_B_DEFAULT_VALUE, COLOR_A_DEFAULT_VALUE))
        {
            watchRefresh(WIDTH_ATTRIBUTE_NAME);
            watchRefresh(BLEND_ATTRIBUTE_NAME);
            watchRefresh(COLOR_ATTRIBUTE_NAME);
        }

        void RendererBeam::collect(
            const Particle::List& inParticles, const PlayInfo& inPlay, Particle::List& outDraws
        ) const
        {
            const float additive = blend.equals(BLEND_TYPE_ADDITIVE) ? BLEND_ADDITIVE_VALUE : BLEND_ALPHA_VALUE;

            if (inPlay.bHasBeam)
            {
                Vec3        delta  = inPlay.destination - inPlay.origin;
                const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
                if (length > LENGTH_MIN_VALUE)
                {
                    const Vec3 position = (inPlay.origin + inPlay.destination) * Vec3(0.5f);

                    Particle beam;
                    beam.position = position;
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
                    Vec3 delta = inPlay.destination - inPlay.origin;

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
