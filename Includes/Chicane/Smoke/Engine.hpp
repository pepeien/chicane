#pragma once

#include <memory>
#include <vector>

#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Smoke.hpp"
#include "Chicane/Smoke/Particle.hpp"
#include "Chicane/Smoke/PlayInfo.hpp"
#include "Chicane/Smoke/System.hpp"

namespace Chicane
{
    class Instance;

    namespace Smoke
    {
        class CHICANE_SMOKE Engine
        {
            friend class ::Chicane::Instance;

        public:
            static Engine& sInstance();

        public:
            Engine(const Engine&)            = delete;
            Engine& operator=(const Engine&) = delete;

            Engine(Engine&&)            = delete;
            Engine& operator=(Engine&&) = delete;

        private:
            Engine();
            ~Engine();

        public:
            void tick(float inDeltaTime);
            void clear();

            void play(const FileSystem::Path& inFilePath, const PlayInfo& inPlay);

            Particle::List getParticles() const;
            std::uint32_t getParticleCount() const;

        private:
            std::vector<std::unique_ptr<System>> m_systems;
        };
    }
}
