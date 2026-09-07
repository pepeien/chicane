#include "Chicane/Core/FileSystem/Listing/Service.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Worker.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        ListingService& ListingService::instance()
        {
            static ListingService service;

            return service;
        }

        void ListingService::enqueue(const Path& inDir)
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_inFlight.find(inDir) != m_inFlight.end())
                {
                    return;
                }

                m_inFlight.insert(inDir);
            }

            Worker::submit(
                [inDir]()
                {
                    Item::List children;
                    try
                    {
                        children = ls(inDir, 1);
                    }
                    catch (...)
                    {
                        children.clear();
                    }

                    ListingService::instance().finish(inDir, std::move(children));
                }
            );
        }

        void ListingService::drain(std::vector<Listing>& outReady)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            outReady.swap(m_ready);

            for (const Listing& listing : outReady)
            {
                m_inFlight.erase(listing.path);
            }
        }

        void ListingService::finish(const Path& inDir, Item::List inChildren)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_ready.push_back({inDir, std::move(inChildren)});
        }
    }
}
