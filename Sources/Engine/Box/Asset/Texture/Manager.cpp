#include "Chicane/Box/Asset/Texture/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        static const Image::Raw EMPTY_RAW_DATA      = {};
        static const Image      EMPTY_COMPILED_DATA = {};

        TextureManager::TextureManager()
            : Super()
        {}

        void TextureManager::onActivation(const String& inId)
        {
            Super::allocate(inId, Image(getInstance(inId)));
        }

        void TextureManager::onDeactivation(const String& inId)
        {
            Super::deallocate(inId);
        }

        void TextureManager::load(const String& inId, const Texture& inAsset)
        {
            if (isLoaded(inId))
            {
                return;
            }

            Super::load(inId, inAsset.getData());
        }

        const Image::Raw& TextureManager::getInstance(const String& inId) const
        {
            if (!isLoaded(inId))
            {
                return EMPTY_RAW_DATA;
            }

            return m_instances.at(inId);
        }

        const Image& TextureManager::getData(const String& inId) const
        {
            if (!isLoaded(inId) || !isAllocated(inId))
            {
                return EMPTY_COMPILED_DATA;
            }

            return m_datum.at(inId);
        }

        std::uint32_t TextureManager::getIndex(const String& inId) const
        {
            auto interator =
                std::find(m_activeIds.begin(), m_activeIds.end(), inId);

            if (interator == m_activeIds.end())
            {
                return 0;
            }

            return static_cast<std::uint32_t>(interator - m_activeIds.begin());
        }
    }
}