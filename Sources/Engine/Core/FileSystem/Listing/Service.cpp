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
            const Path dir = inDir.lexicallyNormal();

            {
                std::lock_guard<std::mutex> lock(m_mutex);

                const auto cached = m_cache.find(dir);
                if (cached != m_cache.end())
                {
                    m_ready.push_back({dir, cached->second});

                    return;
                }

                if (m_inFlight.find(dir) != m_inFlight.end())
                {
                    return;
                }

                m_inFlight.insert(dir);
            }

            Worker::submit(
                [dir]()
                {
                    Item::List children;
                    try
                    {
                        children = ls(dir, 1);
                    }
                    catch (...)
                    {
                        children.clear();
                    }

                    ListingService::instance().finish(dir, std::move(children));
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
            m_cache[inDir] = inChildren;
            m_ready.push_back({inDir, std::move(inChildren)});
        }
    }
}
