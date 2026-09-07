#pragma once

#include <mutex>
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
            static ListingService& instance();

        public:
            ListingService() = default;

        public:
            void enqueue(const Path& inDir);
            void drain(std::vector<Listing>& outReady);

        private:
            void finish(const Path& inDir, Item::List inChildren);

        private:
            std::mutex               m_mutex;
            std::unordered_set<Path> m_inFlight;
            std::vector<Listing>     m_ready;
        };
    }
}
