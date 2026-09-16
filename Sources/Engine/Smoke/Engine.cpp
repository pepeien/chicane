#include "Chicane/Smoke/Engine.hpp"

#include <algorithm>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Effect.hpp"

#include "Chicane/Smoke/Module.hpp"

namespace Chicane
{
    namespace Smoke
    {
        static bool isDeadSystem(const std::unique_ptr<System>& inSystem)
        {
            return !inSystem || !inSystem->isAlive();
        }

        Engine& Engine::getInstance()
        {
            static Engine instance;

            return instance;
        }

        Engine::Engine()
        {}

        Engine::~Engine()
        {
            clear();
        }

        void Engine::tick(float inDeltaTime)
        {
            for (std::unique_ptr<System>& system : m_systems)
            {
                if (system)
                {
                    system->tick(inDeltaTime);
                }
            }

            m_systems.erase(std::remove_if(m_systems.begin(), m_systems.end(), isDeadSystem), m_systems.end());
        }

        void Engine::clear()
        {
            m_systems.clear();
        }

        void Engine::play(const FileSystem::Path& inFilePath, const PlayInfo& inPlay)
        {
            const Box::Effect* asset = Box::load<Box::Effect>(inFilePath);
            if (!asset)
            {
                return;
            }

            const XmlNode root = asset->getXML();
            for (const XmlNode child : root.getChildren())
            {
                if (!child.isElement())
                {
                    continue;
                }

                Module* module = Module::create(child);
                if (!module)
                {
                    continue;
                }

                System* system = dynamic_cast<System*>(module);
                if (!system)
                {
                    delete module;

                    continue;
                }

                system->setPlayInfo(inPlay);
                m_systems.emplace_back(system);
            }
        }

        Particle::List Engine::getParticles() const
        {
            Particle::List result;
            for (const std::unique_ptr<System>& system : m_systems)
            {
                if (system)
                {
                    system->collect(result);
                }
            }

            return result;
        }

        std::uint32_t Engine::getParticleCount() const
        {
            return static_cast<std::uint32_t>(getParticles().size());
        }
    }
}
