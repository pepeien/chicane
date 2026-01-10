#include "Chicane/Screech/Sound.hpp"

#include "Chicane/Screech.hpp"
#include "Chicane/Screech/Sound/Flags.hpp"

#include <miniaudio.h>

namespace Chicane
{
    namespace Screech
    {
        struct _Data
        {
        public:
            ma_decoder decoder;
            ma_sound   sound;
        };

        Sound::Sound(const Raw& inData)
            : Sound()
        {
            if (ma_decoder_init_memory(inData.data(), inData.size(), NULL, &m_data->decoder) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to initialize sound decoder");
            }

            if (ma_sound_init_from_data_source(
                    (ma_engine*)getEngine(),
                    &m_data->decoder,
                    MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC,
                    NULL,
                    &m_data->sound
                ) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to initialize sound from decoder");
            }
        }

        Sound::Sound()
            : m_status(SoundStatus::Stopped),
              m_data(new _Data())
        {}

        Sound::~Sound()
        {
            stop();
        }

        bool Sound::isPlaying() const
        {
            return m_status == SoundStatus::Playing;
        }

        bool Sound::play(float inVolume, float inSpeed, std::function<void()> inCallback)
        {
            if (isPlaying())
            {
                if (!stop())
                {
                    return false;
                }
            }

            ma_sound_set_volume(&m_data->sound, inVolume);
            ma_sound_set_pitch(&m_data->sound, inSpeed);
            ma_sound_set_end_callback(
                &m_data->sound,
                [](void* pUserData, ma_sound* pSound) {
                    Sound* context = (Sound*)pUserData;
                    context->stop();
                },
                this
            );

            m_status = SoundStatus::Playing;

            if (ma_sound_start(&m_data->sound) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to start the sound playback");
            }

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

            if (ma_sound_stop(&m_data->sound) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to stop the sound playback");
            }

            return true;
        }
    }
}