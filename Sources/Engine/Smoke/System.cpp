#include "Chicane/Smoke/System.reflected.hpp"

#include <algorithm>

#include "Chicane/Core/Xml.hpp"
#include "Chicane/Smoke/Spawn.hpp"
#include "Chicane/Smoke/System.hpp"

namespace Chicane
{
    namespace Smoke
    {
        static bool isDeadParticle(const Particle& inParticle)
        {
            return !inParticle.bAlive;
        }

        System::System(const XmlNode& inNode)
            : System()
        {
            parse(inNode);

            for (const XmlNode child : getSource().getChildren())
            {
                if (!child.isElement())
                {
                    continue;
                }

                if (Module* module = Module::create(child))
                {
                    m_modules.emplace_back(module);
                }
            }
        }

        void System::onAttributeRefresh()
        {
            m_tag    = getSource().getName();
            name     = getString(NAME_ATTRIBUTE_NAME, name);
            duration = getFloat(DURATION_ATTRIBUTE_NAME, duration);
            looping  = getBool(LOOPING_ATTRIBUTE_NAME, looping);
        }

        System::System()
            : Module(),
              name(""),
              duration(0.0f),
              looping(false),
              m_play({}),
              m_age(0.0f),
              m_spawnAccumulator(0.0f),
              m_bBurst(false)
        {
            watchRefresh(NAME_ATTRIBUTE_NAME);
            watchRefresh(DURATION_ATTRIBUTE_NAME);
            watchRefresh(LOOPING_ATTRIBUTE_NAME);
        }

        System::~System() = default;

        void System::setPlayInfo(const PlayInfo& inValue)
        {
            m_play = inValue;
        }

        bool System::isAlive() const
        {
            if (looping)
            {
                return true;
            }

            if (duration > 0.0f && m_age < duration)
            {
                return true;
            }

            for (const Particle& particle : m_particles)
            {
                if (particle.bAlive)
                {
                    return true;
                }
            }

            return false;
        }

        void System::tick(float inDeltaTime)
        {
            tick(inDeltaTime, m_particles, m_play);
        }

        void System::collect(Particle::List& outDraws) const
        {
            bool bHasRenderer = false;

            for (const std::unique_ptr<Module>& module : m_modules)
            {
                if (!module)
                {
                    continue;
                }

                const std::size_t before = outDraws.size();
                module->collect(m_particles, m_play, outDraws);
                if (outDraws.size() > before)
                {
                    bHasRenderer = true;
                }
            }

            if (!bHasRenderer)
            {
                for (const Particle& particle : m_particles)
                {
                    if (particle.bAlive)
                    {
                        outDraws.push_back(particle);
                    }
                }
            }
        }

        void System::tick(float inDeltaTime, Particle::List& outParticles, const PlayInfo& inPlay)
        {
            m_age += inDeltaTime;

            Spawn* spawn = nullptr;
            for (std::unique_ptr<Module>& module : m_modules)
            {
                if (!module)
                {
                    continue;
                }

                if (Spawn* found = dynamic_cast<Spawn*>(module.get()))
                {
                    spawn = found;
                }
            }

            if (spawn)
            {
                if (!m_bBurst && spawn->burst > 0)
                {
                    spawn->emit(outParticles, inPlay, spawn->burst);
                    m_bBurst = true;
                }

                const bool bCanSpawn = looping || duration <= 0.0f || m_age <= duration;
                if (bCanSpawn && spawn->rate > 0.0f)
                {
                    m_spawnAccumulator += spawn->rate * inDeltaTime;
                    const std::uint32_t count = static_cast<std::uint32_t>(m_spawnAccumulator);
                    if (count > 0)
                    {
                        m_spawnAccumulator -= static_cast<float>(count);
                        spawn->emit(outParticles, inPlay, count);
                    }
                }
            }
            else
            {
                m_bBurst = true;
            }

            for (std::unique_ptr<Module>& module : m_modules)
            {
                if (!module || dynamic_cast<Spawn*>(module.get()))
                {
                    continue;
                }

                module->tick(inDeltaTime, outParticles, inPlay);
            }

            for (Particle& particle : outParticles)
            {
                if (!particle.bAlive)
                {
                    continue;
                }

                particle.age += inDeltaTime;
                if (particle.age >= particle.lifetime)
                {
                    particle.bAlive = false;

                    continue;
                }

                const float alpha = particle.lifetime > 0.0f ? std::min(particle.age / particle.lifetime, 1.0f) : 1.0f;
                particle.position = particle.position + particle.velocity * inDeltaTime;
                particle.size     = Vec2(
                    particle.sizeStart.x + (particle.sizeEnd.x - particle.sizeStart.x) * alpha,
                    particle.sizeStart.y + (particle.sizeEnd.y - particle.sizeStart.y) * alpha
                );

                particle.color = Vec4(
                    particle.colorStart.x + (particle.colorEnd.x - particle.colorStart.x) * alpha,
                    particle.colorStart.y + (particle.colorEnd.y - particle.colorStart.y) * alpha,
                    particle.colorStart.z + (particle.colorEnd.z - particle.colorStart.z) * alpha,
                    particle.colorStart.w + (particle.colorEnd.w - particle.colorStart.w) * alpha
                );
            }

            outParticles.erase(
                std::remove_if(outParticles.begin(), outParticles.end(), isDeadParticle),
                outParticles.end()
            );
        }
    }
}
