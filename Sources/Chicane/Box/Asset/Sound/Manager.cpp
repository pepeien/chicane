#include "Chicane/Box/Asset/Sound/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sound
        {
            static const Audio::Raw      EMPTY_RAW_DATA      = {};
            static const Audio::Instance EMPTY_COMPILED_DATA = {};

            Manager::Manager()
                : Super()
            {}

            void Manager::onActivation(const std::string& inId)
            {
                Super::allocate(inId, Audio::Instance(getInstance(inId)));
            }

            void Manager::onDeactivation(const std::string& inId)
            {
                Super::deallocate(inId);
            }

            void Manager::load(const Sound::Instance& inAsset)
            {
                const std::string& id = inAsset.getId();

                if (isLoaded(id))
                {
                    return;
                }

                Super::load(id, inAsset.getData());
            }

            const Audio::Raw& Manager::getInstance(const std::string& inId) const
            {
                if (!isLoaded(inId))
                {
                    return EMPTY_RAW_DATA;
                }

                return m_instances.at(inId);
            }

            Audio::Instance Manager::getData(const std::string& inId) const
            {
                if (!isLoaded(inId) || !isAllocated(inId))
                {
                    return EMPTY_COMPILED_DATA;
                }

                return m_datum.at(inId);
            }
        }
    }
}