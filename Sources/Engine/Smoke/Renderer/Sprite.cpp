#include "Chicane/Smoke/Renderer/Sprite.reflected.hpp"

#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Smoke
    {
        RendererSprite::RendererSprite(const XmlNode& inNode)
            : RendererSprite()
        {
            parse(inNode);
        }

        void RendererSprite::onAttributeRefresh()
        {
            m_tag   = getSource().getName();
            blend   = getString(BLEND_ATTRIBUTE_NAME, blend);
            texture = getString(TEXTURE_ATTRIBUTE_NAME, texture);
        }

        RendererSprite::RendererSprite()
            : Module(),
              blend(BLEND_TYPE_ADDITIVE),
              texture("")
        {
            watchRefresh(BLEND_ATTRIBUTE_NAME);
            watchRefresh(TEXTURE_ATTRIBUTE_NAME);
        }

        void RendererSprite::collect(const Particle::List& inParticles, const PlayInfo&, Particle::List& outDraws) const
        {
            const float additive = blend.equals(BLEND_TYPE_ADDITIVE) ? BLEND_ADDITIVE_VALUE : BLEND_ALPHA_VALUE;

            for (const Particle& particle : inParticles)
            {
                if (!particle.bAlive)
                {
                    continue;
                }

                Particle draw = particle;
                draw.additive = additive;
                draw.axis     = Vec3::sZero();
                outDraws.push_back(draw);
            }
        }
    }
}
