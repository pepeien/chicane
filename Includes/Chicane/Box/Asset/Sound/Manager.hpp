#pragma once

#include "Chicane/Box/Asset/Sound.hpp"
#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX SoundManager : public Box::Manager<Audio::Raw, Audio::Instance>
        {
        public:
            SoundManager();
            virtual ~SoundManager() = default;

        protected:
            void onActivation(const String& inId) override;
            void onDeactivation(const String& inId) override;

        public:
            // Setup
            void load(const Sound& inAudio);

            // Data
            const Audio::Raw& getInstance(const String& inId) const;
            Audio::Instance getData(const String& inId) const;
        };
    }
}