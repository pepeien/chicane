#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem/Listing.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        class CHICANE_CORE ListingService
        {
        public:
            static inline ListingService& sInstance()
            {
                static ListingService result;

                return result;
            }

        public:
            ListingService() = default;

        public:
            void enqueue(const Path& inDir);
            void drain(std::vector<Listing>& outReady);

        private:
            void finish(const Path& inDir, Item::List inChildren);

        private:
            std::mutex                           m_mutex;
            std::unordered_set<Path>             m_inFlight;
            std::unordered_map<Path, Item::List> m_cache;
            std::vector<Listing>                 m_ready;
        };
    }
}
