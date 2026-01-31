#include "Chicane/Screech/Sound.hpp"

#include <miniaudio.h>

#include "Chicane/Screech.hpp"
#include "Chicane/Screech/Sound/Flags.hpp"

namespace Chicane
{
    namespace Screech
    {
        class _Data
        {
        public:
            ma_decoder decoder;
            ma_sound   sound;
            Sound::Raw data;
        };

        Sound::Sound()
            : m_status(SoundStatus::Stopped),
              m_data(std::make_unique<_Data>())
        {}

        Sound::~Sound()
        {
            ma_sound_uninit(&m_data->sound);
            ma_decoder_uninit(&m_data->decoder);
        }

        void Sound::setData(const Raw& inValue)
        {
            if (!stop())
            {
                return;
            }

            if (inValue.size() <= 0 || inValue.data() == nullptr)
            {
                throw std::runtime_error("Sound source is empty");
            }

            m_data->data = inValue;

            if (ma_decoder_init_memory(m_data->data.data(), m_data->data.size(), NULL, &m_data->decoder) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to initialize sound decoder");
            }

            if (ma_sound_init_from_data_source(
                    (ma_engine*)getEngine(),
                    &m_data->decoder,
                    MA_SOUND_FLAG_DECODE,
                    NULL,
                    &m_data->sound
                ) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to initialize sound from decoder");
            }
        }

        bool Sound::isPlaying() const
        {
            return m_status == SoundStatus::Playing;
        }

        bool Sound::play(float inVolume, float inSpeed, std::function<void()> inCallback)
        {
            if (!stop())
            {
                return false;
            }

            ma_sound_set_volume(&m_data->sound, inVolume);
            ma_sound_set_pitch(&m_data->sound, inSpeed);
            ma_sound_set_end_callback(
                &m_data->sound,
                [](void* pUserData, ma_sound*) {
                    Sound* context    = (Sound*)pUserData;
                    context->m_status = SoundStatus::Stopped;
                },
                this
            );

            if (ma_sound_start(&m_data->sound) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to start the sound playback");
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

            if (ma_sound_stop(&m_data->sound) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to pause the sound playback");
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

            if (ma_sound_stop(&m_data->sound) != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to stop the sound playback");
            }

            m_status = SoundStatus::Stopped;

            return true;
        }
    }
}