#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/Audio.hpp"
#include "Chicane/Core/Audio.hpp"
#include "Chicane/Renderer/Manager.hpp"

namespace Chicane
{
    namespace Audio
    {
        class Manager : public Chicane::Manager::Instance<Audio::Instance, Audio::Data>
        {
        public:
            Manager();
            ~Manager();

        protected:
            void onActivation(const std::string& inId) override;
            void onDeactivation(const std::string& inId) override;

        public:
            // Setup
            void load(const std::string& inId, const Box::Audio* inAudio);

            // Data
            const Audio::Instance& getInstance(const std::string& inId) const;
            const Audio::Data& getData(const std::string& inId) const;
        };
    }
}