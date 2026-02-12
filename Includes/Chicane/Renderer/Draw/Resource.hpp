#pragma once

#include <unordered_map>

#include "Chicane/Core/Hash.hpp"
#include "Chicane/Core/Hash/Subject.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        template <typename Data, typename Seed>
        struct CHICANE_RENDERER DrawResource
        {
        public:
            using HashTable = std::unordered_map<Hash::Value, Draw::Id>;

        public:
            inline bool isDirty() const { return m_bIsDirty; }

            inline void markAsDirty() { m_bIsDirty = true; }

            inline void markAsClean() { m_bIsDirty = false; }

        protected:
            inline Draw::Id findHash(
                const Data* inData, std::uint32_t inDataSize, const Seed* inSeed, std::uint32_t inSeedSize
            )
            {
                return findHash(hash(inData, inDataSize, inSeed, inSeedSize));
            }

            inline Draw::Id findHash(Hash::Value inValue)
            {
                if (m_hashes.find(inValue) == m_hashes.end())
                {
                    return Draw::UnknownId;
                }

                return m_hashes.at(inValue);
            }

            inline void addHash(
                const Data*   inData,
                std::uint32_t inDataSize,
                const Seed*   inSeed,
                std::uint32_t inSeedSize,
                Draw::Id      inId
            )
            {
                addHash(hash(inData, inDataSize, inSeed, inSeedSize), inId);
            }

            inline void addHash(Hash::Value inValue, Draw::Id inId)
            {
                if (findHash(inValue) > Draw::UnknownId)
                {
                    return;
                }

                m_hashes.emplace(inValue, inId);

                markAsDirty();
            }

            inline Hash::Value hash(
                const Data* inData, std::uint32_t inDataSize, const Seed* inSeed, std::uint32_t inSeedSize
            )
            {
                Hash::Subject data;
                data.data = inData;
                data.size = inDataSize;

                Hash::Subject seed;
                seed.data = inSeed;
                seed.size = inSeedSize;

                return Hash::generate(data, seed);
            }

            void clearHashes() { m_hashes.clear(); }

        private:
            bool      m_bIsDirty = false;

            HashTable m_hashes = {};
        };
    }
}