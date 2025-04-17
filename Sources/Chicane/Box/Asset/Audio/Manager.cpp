#include "Box/Asset/Audio/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Audio
        {
            static const Audio::RawData      EMPTY_INSTANCE = {};
            static const Audio::CompiledData EMPTY_DATA     = {};

            Manager::Manager()
                : Super()
            {}

            Manager::~Manager()
            {
                for (auto& [id, audio] : m_datum)
                {
                    if (audio.bufferData)
                    {
                        SDL_free(audio.bufferData);
                        audio.bufferData = nullptr;
                    }
                }
            }

            void Manager::onActivation(const std::string& inId)
            {
                Audio::CompiledData data = {};

                Audio::RawData instance = getInstance(inId);

                bool wasLoaded = SDL_LoadWAV_IO(
                    SDL_IOFromMem((void*)instance.data(), sizeof(unsigned char) * instance.size()),
                    true,
                    &data.specification,
                    &data.bufferData,
                    &data.bufferLength
                );

                if (!wasLoaded)
                {
                    throw std::runtime_error(SDL_GetError());
                }

                data.stream = SDL_OpenAudioDeviceStream(
                    SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                    &data.specification,
                    nullptr,
                    nullptr
                );

                if (!data.stream)
                {
                    throw std::runtime_error(SDL_GetError());
                }

                Super::allocate(inId, data);
            }

            void Manager::onDeactivation(const std::string& inId)
            {
                Audio::CompiledData& audio = m_datum.at(inId);

                SDL_ClearAudioStream(audio.stream);
                audio.stream = nullptr;

                SDL_free(audio.bufferData);
                audio.bufferData = nullptr;

                Super::deallocate(inId);
            }

            void Manager::load(const std::string& inId, const Audio::Instance* inAsset)
            {
                if (isLoaded(inId))
                {
                    return;
                }

                Super::load(inId, inAsset->getData());
                Super::activate(inId);
            }

            const Audio::RawData& Manager::getInstance(const std::string& inId) const
            {
                if (!isLoaded(inId))
                {
                    return EMPTY_INSTANCE;
                }

                return m_instances.at(inId);
            }

            const Audio::CompiledData& Manager::getData(const std::string& inId) const
            {
                if (!isLoaded(inId) || !isAllocated(inId))
                {
                    return EMPTY_DATA;
                }

                return m_datum.at(inId);
            }
        }
    }
}