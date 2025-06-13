#include "Chicane/Core/Audio/Instance.hpp"

#include <SDL3/SDL_audio.h>

namespace Chicane
{
    namespace Audio
    {
        Instance::Instance(const FileSystem::Path& inFilepath)
            : Instance(FileSystem::readUnsigned(inFilepath))
        {}

        Instance::Instance(const Raw& inData)
            : Instance()
        {
            SDL_AudioSpec specification;

            if (
                !SDL_LoadWAV_IO(
                    SDL_IOFromMem((void*)inData.data(), sizeof(unsigned char) * inData.size()),
                    true,
                    &specification,
                    &m_bufferData,
                    &m_bufferLength
                )
            )
            {
                throw std::runtime_error(SDL_GetError());
            }

            m_specification.format    = specification.format;
            m_specification.channels  = specification.channels;
            m_specification.frequency = specification.freq;
        }

        Instance::Instance()
            : m_vendor(Vendor::Undefined),
            m_specification({}),
            m_bufferLength(0),
            m_bufferData(nullptr),
            m_stream(nullptr)
        {}

        Instance::Instance(const Instance& inInstance)
            : m_vendor(inInstance.m_vendor),
            m_specification(inInstance.m_specification),
            m_bufferLength(inInstance.m_bufferLength),
            m_bufferData(nullptr),
            m_stream(nullptr)
        {
            if (m_bufferLength > 0 && inInstance.m_bufferData)
            {
                m_bufferData = new std::uint8_t[m_bufferLength];
                std::memcpy(m_bufferData, inInstance.m_bufferData, m_bufferLength);
            }
        }

        Instance::~Instance()
        {
            m_stream = nullptr;

            SDL_free(m_bufferData);
            m_bufferData = nullptr;
        }

        void Instance::play()
        {
            if (!m_stream)
            {
                SDL_AudioSpec specification;
                specification.format   = (SDL_AudioFormat) m_specification.format;
                specification.channels = m_specification.channels;
                specification.freq     = m_specification.frequency;

                m_stream = SDL_OpenAudioDeviceStream(
                    SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                    &specification,
                    nullptr,
                    nullptr
                );

                if (!m_stream)
                {
                    throw std::runtime_error(SDL_GetError());
                }
            }

            SDL_AudioStream* stream = static_cast<SDL_AudioStream*>(m_stream);
            SDL_ClearAudioStream(stream);
            SDL_ResumeAudioStreamDevice(stream);
            SDL_PutAudioStreamData(stream, m_bufferData, m_bufferLength);    
        }
    }
}