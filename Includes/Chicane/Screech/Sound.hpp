#pragma once

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Screech/Essential.hpp"
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
            Sound(const Raw& inData, SoundVendor inVendor);
            Sound();

        public:
            bool isPlaying() const;
            bool play();

            bool isPaused() const;
            bool pause();

            bool isStopped() const;
            bool stop();

        private:
            SoundStatus m_status;
            SoundVendor m_vendor;
        };
    }
}