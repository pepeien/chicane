#include "Chicane/Screech/Sound.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "Chicane/Screech/Device.hpp"

namespace Chicane
{
    namespace Screech
    {
        Sound::Sound(const Raw& inData, SoundVendor inVendor)
            : Sound()
        {
            m_vendor = inVendor;
        }

        Sound::Sound()
            : m_status(SoundStatus::Stopped),
            m_vendor(SoundVendor::Null)
        {}

        bool Sound::isPlaying() const
        {
            return m_status == SoundStatus::Playing;
        }

        bool Sound::play()
        {
            if (isPlaying())
            {
                if (!stop())
                {
                    return false;
                }
            }

            m_status = SoundStatus::Playing;

            return true;
        }

        bool Sound::isPaused() const
        {
            return m_status == SoundStatus::Paused;
        }

        bool Sound::pause()
        {
            if (isPaused())
            {
                return true;
            }

            m_status = SoundStatus::Paused;

            return true;
        }

        bool Sound::isStopped() const
        {
            return m_status == SoundStatus::Stopped;
        }

        bool Sound::stop()
        {
            if (isStopped())
            {
                return true;
            }

            m_status = SoundStatus::Stopped;

            return true;
        }
    }
}