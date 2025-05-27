#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Manager.hpp"
#include "Chicane/Box/Asset/Sound/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sound
        {
            class CHICANE_BOX Manager : public Box::Manager::Instance<Audio::Raw, Audio::Instance>
            {
            public:
                Manager();
                virtual ~Manager() = default;

            protected:
                void onActivation(const std::string& inId) override;
                void onDeactivation(const std::string& inId) override;

            public:
                // Setup
                void load(const Sound::Instance& inAudio);

                // Data
                const Audio::Raw& getInstance(const std::string& inId) const;
                Audio::Instance getData(const std::string& inId) const;
            };
        }
    }
}