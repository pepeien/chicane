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
                void onActivation(const String& inId) override;
                void onDeactivation(const String& inId) override;

            public:
                // Setup
                void load(const Sound::Instance& inAudio);

                // Data
                const Audio::Raw& getInstance(const String& inId) const;
                Audio::Instance getData(const String& inId) const;
            };
        }
    }
}