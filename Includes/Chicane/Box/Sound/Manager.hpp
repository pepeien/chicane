#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Sound.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX SoundManager : public Box::Manager<Sound, Sound::Raw>
        {
        public:
            SoundManager();
            ~SoundManager() = default;

        protected:
            void onLoad(const String& inId, const Sound& inData) override;
        };
    }
}