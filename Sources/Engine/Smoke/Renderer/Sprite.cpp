#include "Chicane/Smoke/Renderer/Sprite.reflected.hpp"

#include "Chicane/Smoke/Parse.hpp"
#include "Chicane/Smoke/Renderer/Sprite.hpp"

namespace Chicane
{
    namespace Smoke
    {

        RendererSprite::RendererSprite(const pugi::xml_node& inNode)
            : RendererSprite()
        {
            m_tag   = inNode.name();
            blend   = parseString(inNode, BLEND_ATTRIBUTE_NAME, blend);
            texture = parseString(inNode, TEXTURE_ATTRIBUTE_NAME, texture);
        }

        RendererSprite::RendererSprite()
            : Module(),
              blend(BLEND_TYPE_ADDITIVE),
              texture("")
        {}

        void RendererSprite::collect(const Particle::List& inParticles, const PlayInfo&, Particle::List& outDraws) const
        {
            const float additive = blend.equals(BLEND_TYPE_ADDITIVE, BLEND_TYPE_ADD, BLEND_TYPE_ADDITIVE_LOWER)
                                       ? BLEND_ADDITIVE_VALUE
                                       : BLEND_ALPHA_VALUE;

            for (const Particle& particle : inParticles)
            {
                if (!particle.bAlive)
                {
                    continue;
                }

                Particle draw = particle;
                draw.additive = additive;
                draw.axis     = Vec3::Zero();
                outDraws.push_back(draw);
            }
        }
    }
}
