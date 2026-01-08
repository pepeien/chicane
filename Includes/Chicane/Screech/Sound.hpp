#pragma once

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Screech.hpp"
#include "Chicane/Screech/Sound/Status.hpp"
#include "Chicane/Screech/Sound/Vendor.hpp"

namespace Chicane
{
    namespace Screech
    {
        struct CHICANE_SCREECH Sound
        {
        public:
            using Raw = std::vector<unsigned char>;

        public:
            Sound(const Raw& inData);
            Sound();

            ~Sound();

        public:
            bool isPlaying() const;
            bool play(float inVolume = 0.7f, float inSpeed = 1.0f, std::function<void ()> inCallback = nullptr);

            bool isPaused() const;
            bool pause();

            bool isStopped() const;
            bool stop();

        private:
            SoundStatus   m_status;

            struct _Data* m_data;
        };
    }
}