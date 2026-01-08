#include "Chicane/Box/Asset/Sound/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        static const Sound::Raw EMPTY_RAW_DATA = {};

        SoundManager::SoundManager()
            : Super()
        {}

        void SoundManager::onActivation(const String& inId)
        {
            Super::allocate(inId, getInstance(inId));
        }

        void SoundManager::onDeactivation(const String& inId)
        {
            Super::deallocate(inId);
        }

        void SoundManager::load(const Sound& inAsset)
        {
            const String& id = inAsset.getId();

            if (isLoaded(id))
            {
                return;
            }

            Super::load(id, inAsset.getData());
        }

        const Sound::Raw& SoundManager::getInstance(const String& inId) const
        {
            if (!isLoaded(inId))
            {
                return EMPTY_RAW_DATA;
            }

            return m_instances.at(inId);
        }

        const Sound::Raw& SoundManager::getData(const String& inId) const
        {
            if (!isLoaded(inId) || !isAllocated(inId))
            {
                return EMPTY_RAW_DATA;
            }

            return m_datum.at(inId);
        }
    }
}