#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Smoke.hpp"
#include "Chicane/Smoke/Module.hpp"
#include "Chicane/Smoke/Particle.hpp"
#include "Chicane/Smoke/PlayInfo.hpp"

namespace Chicane
{
    namespace Smoke
    {
        CH_TYPE(Manual)
        class CHICANE_SMOKE System : public Module
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "System";

            // Attributes
            static constexpr inline const char* NAME_ATTRIBUTE_NAME     = "name";
            static constexpr inline const char* DURATION_ATTRIBUTE_NAME = "duration";
            static constexpr inline const char* LOOPING_ATTRIBUTE_NAME  = "looping";

        public:
            CH_CONSTRUCTOR()
            System(const XmlNode& inNode);

            System();

            System(const System&)            = delete;
            System& operator=(const System&) = delete;

            System(System&&)            = default;
            System& operator=(System&&) = default;

            ~System() override;

        public:
            void onAttributeRefresh() override;
            void setPlayInfo(const PlayInfo& inValue);
            bool isAlive() const;

            void tick(float inDeltaTime);
            void collect(Particle::List& outDraws) const;

            void tick(float inDeltaTime, Particle::List& outParticles, const PlayInfo& inPlay) override;

        public:
            CH_FIELD()
            String name;

            CH_FIELD()
            float duration;

            CH_FIELD()
            bool looping;

        private:
            PlayInfo       m_play;
            Module::List   m_modules;
            Particle::List m_particles;
            float          m_age;
            float          m_spawnAccumulator;
            bool           m_bBurst;
        };
    }
}
