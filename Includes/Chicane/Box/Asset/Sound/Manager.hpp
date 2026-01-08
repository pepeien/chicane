#pragma once

#include "Chicane/Box/Asset/Sound.hpp"
#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX SoundManager : public Box::Manager<Sound::Raw, Sound::Raw>
        {
        public:
            SoundManager();
            virtual ~SoundManager() = default;

        protected:
            void onActivation(const String& inId) override;
            void onDeactivation(const String& inId) override;

        public:
            // Setup
            void load(const Sound& inAsset);

            // Data
            const Sound::Raw& getInstance(const String& inId) const;
            const Sound::Raw& getData(const String& inId) const;
        };
    }
}