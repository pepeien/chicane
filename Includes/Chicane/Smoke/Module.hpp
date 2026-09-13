#pragma once

#include <memory>
#include <vector>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Smoke.hpp"
#include "Chicane/Smoke/Particle.hpp"
#include "Chicane/Smoke/PlayInfo.hpp"

namespace Chicane
{
    namespace Smoke
    {
        CH_TYPE(Manual)
        class CHICANE_SMOKE Module
        {
        public:
            using List = std::vector<std::unique_ptr<Module>>;

        public:
            static Module* create(const pugi::xml_node& inNode);

        public:
            Module();

            CH_CONSTRUCTOR()
            Module(const pugi::xml_node& inNode);

            virtual ~Module();

        public:
            virtual void tick(float inDeltaTime, Particle::List& outParticles, const PlayInfo& inPlay);
            virtual void collect(const Particle::List& inParticles, const PlayInfo& inPlay, Particle::List& outDraws)
                const;

            const String& getTag() const;

        protected:
            String m_tag;
        };
    }
}
